#ifndef JME_SharedMemory_h__
#define JME_SharedMemory_h__

/********************************************************************
	created:	2015/06/19
	author:		huangzhi
	
	purpose:	通过共享内存创建对象， 仅支持固定大小的对象创建
	warning:	一块共享内存 在 一个进程中仅能有一个映射
*********************************************************************/


#include <map>
#include <string>
#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"

#include "JME_GLog.h"

using namespace std;
namespace JMEngine
{
	namespace game
	{
		template<class T>
		class JME_SHM
		{
		public:
			typedef map<string, boost::interprocess::mapped_region*> MappedRegion;

			static boost::shared_ptr<T> create(boost::interprocess::create_only_t, const char* name, boost::interprocess::mode_t mode);
			static boost::shared_ptr<T> create(boost::interprocess::open_only_t, const char* name, boost::interprocess::mode_t mode);

		private:
			static void destory(T* p, const char* name);
			static void saveMappedRegion(const char* name, boost::interprocess::mapped_region* mmap);
			static void deleteMappedRegion(const char* name);
			static bool existMappedRegion(const char* name);
		private:
			static MappedRegion _MappedRegion;
		};

		template<class T>
		typename JME_SHM<T>::MappedRegion JME_SHM<T>::_MappedRegion;

		template<class T>
		bool JMEngine::game::JME_SHM<T>::existMappedRegion(const char* name)
		{
			auto it = _MappedRegion.find(name);
			if (it != _MappedRegion.end())
				return true;
			return false;
		}

		template<class T>
		void JMEngine::game::JME_SHM<T>::deleteMappedRegion(const char* name)
		{
			auto it = _MappedRegion.find(name);
			if (it != _MappedRegion.end())
			{
				auto mmap = it->second;
				delete mmap;

				_MappedRegion.erase(name);
			}
		}

		template<class T>
		void JMEngine::game::JME_SHM<T>::saveMappedRegion(const char* name, boost::interprocess::mapped_region* mmap)
		{
			auto result = _MappedRegion.insert(make_pair(name, mmap));
		}

		template<class T>
		void JMEngine::game::JME_SHM<T>::destory(T* p, const char* name)
		{
			JMEngine::game::JME_SHM<T>::deleteMappedRegion(name);
			boost::interprocess::shared_memory_object::remove(name);
		}

		template<class T>
		boost::shared_ptr<T> JMEngine::game::JME_SHM<T>::create(boost::interprocess::create_only_t, const char* name, boost::interprocess::mode_t mode)
		{
			try
			{
				boost::interprocess::shared_memory_object shm(boost::interprocess::create_only, name, mode);
				shm.truncate(sizeof(T));

				auto mmap = new boost::interprocess::mapped_region(shm, mode);
				JMEngine::game::JME_SHM<T>::saveMappedRegion(name, mmap);

				auto ptr = boost::shared_ptr<T>(new(mmap->get_address()) T, boost::bind(JME_SHM<T>::destory, _1, name));
				return ptr;
			}
			catch(boost::interprocess::interprocess_exception& e)
			{
				LOGE("Create shared memory with name [ %s ] error ==> [ %s ]", name, e.what());

				return boost::shared_ptr<T>();
			}
		}

		template<class T>
		boost::shared_ptr<T> JMEngine::game::JME_SHM<T>::create(boost::interprocess::open_only_t, const char* name, boost::interprocess::mode_t mode)
		{
			try
			{

				if (JMEngine::game::JME_SHM<T>::existMappedRegion(name))
				{
					LOGE("The shared memory with name [ %s ] had been opened", name);
					return boost::shared_ptr<T>();
				}
				boost::interprocess::shared_memory_object shm(boost::interprocess::open_only, name, mode);

				auto mmap = new boost::interprocess::mapped_region(shm, mode);
				JMEngine::game::JME_SHM<T>::saveMappedRegion(name, mmap);

				auto ptr = boost::shared_ptr<T>(new(mmap->get_address()) T, boost::bind(JME_SHM<T>::destory, _1, name));
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
