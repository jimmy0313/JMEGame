#ifndef JME_SharedMemory_h__
#define JME_SharedMemory_h__

/********************************************************************
	created:	2015/06/19
	author:		huangzhi
	
	purpose:	通过共享内存创建对象， 仅支持固定大小的对象创建, 带有 虚函数/虚基类 的类无法使用
	warning:	一块共享内存 在 一个进程中仅能有一个映射， 具体 对象限制参见boost::interprocess
*********************************************************************/


#include <map>
#include <string>
#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"
#include "boost/thread/recursive_mutex.hpp"

#include "JME_GLog.h"

using namespace std;
namespace JMEngine
{
	namespace game
	{
		template<class T>
		class JME_SharedMemory
		{
		public:
			typedef map<string, boost::interprocess::mapped_region*> MappedRegion;

			//主要用于cache, 用于负责cache的进程 创建共享内存 设置正确数据(如 从内存读取)
			static boost::shared_ptr<T> create(boost::interprocess::create_only_t, const char* name, boost::interprocess::mode_t mode);

			//主要用于logic，用于logic进程， 对数据进行修改
			static boost::shared_ptr<T> create(boost::interprocess::open_only_t, const char* name, boost::interprocess::mode_t mode);

			static bool existMappedRegion(const char* name);
		private:
			static void destory(const char* name, bool remove_memory);
			static void saveMappedRegion(const char* name, boost::interprocess::mapped_region* mmap);
			static void deleteMappedRegion(const char* name);
		private:
			static boost::recursive_mutex _mutex;
			static MappedRegion _MappedRegion;
		};

		template<class T>
		typename JME_SharedMemory<T>::MappedRegion JME_SharedMemory<T>::_MappedRegion;
		template<class T>
		typename boost::recursive_mutex JME_SharedMemory<T>::_mutex;

		template<class T>
		bool JMEngine::game::JME_SharedMemory<T>::existMappedRegion(const char* name)
		{
			boost::recursive_mutex::scoped_lock lock(_mutex);

			auto it = _MappedRegion.find(name);
			if (it != _MappedRegion.end())
				return true;
			return false;
		}

		template<class T>
		void JMEngine::game::JME_SharedMemory<T>::deleteMappedRegion(const char* name)
		{
			boost::recursive_mutex::scoped_lock lock(_mutex);

			auto it = _MappedRegion.find(name);
			if (it != _MappedRegion.end())
			{
				auto mmap = it->second;
				delete mmap;

				_MappedRegion.erase(name);
			}
		}

		template<class T>
		void JMEngine::game::JME_SharedMemory<T>::saveMappedRegion(const char* name, boost::interprocess::mapped_region* mmap)
		{
			boost::recursive_mutex::scoped_lock lock(_mutex);

			auto result = _MappedRegion.insert(make_pair(name, mmap));
		}

		template<class T>
		void JMEngine::game::JME_SharedMemory<T>::destory(const char* name, bool remove_memory)
		{
			JMEngine::game::JME_SharedMemory<T>::deleteMappedRegion(name);
			if (remove_memory)
			{
				boost::interprocess::shared_memory_object::remove(name);
			}
		}

		template<class T>
		boost::shared_ptr<T> JMEngine::game::JME_SharedMemory<T>::create(boost::interprocess::create_only_t, const char* name, boost::interprocess::mode_t mode)
		{
			try
			{
				//移除可能存在老共享内存
				JMEngine::game::JME_SharedMemory<T>::deleteMappedRegion(name);
				boost::interprocess::shared_memory_object::remove(name);

				//创建新的共享内存, 并分配大小
				boost::interprocess::shared_memory_object shm(boost::interprocess::create_only, name, mode);
				shm.truncate(sizeof(T));

				//创建内存映射
				auto mmap = new boost::interprocess::mapped_region(shm, mode);

				//保存内存映射信息
				JMEngine::game::JME_SharedMemory<T>::saveMappedRegion(name, mmap);

				//通过共享内存 创建shared_ptr<T>对象
				//共享内存， 谁创建， 谁销毁
				auto ptr = boost::shared_ptr<T>(new(mmap->get_address()) T, boost::bind(JME_SharedMemory<T>::destory, name, true));
				return ptr;
			}
			catch(boost::interprocess::interprocess_exception& e)
			{
				LOGE("Create shared memory with name [ %s ] error ==> [ %s ]", name, e.what());

				return boost::shared_ptr<T>();
			}
		}

		template<class T>
		boost::shared_ptr<T> JMEngine::game::JME_SharedMemory<T>::create(boost::interprocess::open_only_t, const char* name, boost::interprocess::mode_t mode)
		{
			try
			{
				//检测当前进程是否已经打开共享内存
				if (JMEngine::game::JME_SharedMemory<T>::existMappedRegion(name))
				{
					LOGE("The shared memory with name [ %s ] had been opened", name);
					return boost::shared_ptr<T>();
				}
				//打开共享内存
				boost::interprocess::shared_memory_object shm(boost::interprocess::open_only, name, mode);

				//创建内存映射
				auto mmap = new boost::interprocess::mapped_region(shm, mode);

				//如何共享内存分配内存大小 与 对象大小不一致， 对象被修改（大小一致 并不代表 对象没有被修改）
				if (sizeof(T) != mmap->get_size())
				{
					//移除内存映射
					delete mmap;
					return boost::shared_ptr<T>();
				}
				//保存内存映射
				JMEngine::game::JME_SharedMemory<T>::saveMappedRegion(name, mmap);

				//直接将共享内存 转为对象， 不再调用构造函数
				auto ptr = boost::shared_ptr<T>((T*)mmap->get_address(), boost::bind(JME_SharedMemory<T>::destory, name, false));
				return ptr;
			}
			catch(boost::interprocess::interprocess_exception& e)
			{
				LOGE("Open shared memory with name [ %s ] error ==> [ %s ]", name, e.what());

				return boost::shared_ptr<T>();
			}
		}

	}
}
#endif // JME_SharedMemory_h__
