#ifndef JME_SharedMemory_h__
#define JME_SharedMemory_h__

/********************************************************************
	created:	2015/06/19
	author:		huangzhi
	
	purpose:	ͨ�������ڴ洴������ ��֧�̶ֹ���С�Ķ��󴴽�, ���� �麯��/����� �����޷�ʹ��
	warning:	һ�鹲���ڴ� �� һ�������н�����һ��ӳ�䣬 ���� �������Ʋμ�boost::interprocess
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

			//��Ҫ����cache, ���ڸ���cache�Ľ��� ���������ڴ� ������ȷ����(�� ���ڴ��ȡ)
			static boost::shared_ptr<T> create(boost::interprocess::create_only_t, const char* name, boost::interprocess::mode_t mode);

			//��Ҫ����logic������logic���̣� �����ݽ����޸�
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
				//�Ƴ����ܴ����Ϲ����ڴ�
				JMEngine::game::JME_SharedMemory<T>::deleteMappedRegion(name);
				boost::interprocess::shared_memory_object::remove(name);

				//�����µĹ����ڴ�, �������С
				boost::interprocess::shared_memory_object shm(boost::interprocess::create_only, name, mode);
				shm.truncate(sizeof(T));

				//�����ڴ�ӳ��
				auto mmap = new boost::interprocess::mapped_region(shm, mode);

				//�����ڴ�ӳ����Ϣ
				JMEngine::game::JME_SharedMemory<T>::saveMappedRegion(name, mmap);

				//ͨ�������ڴ� ����shared_ptr<T>����
				//�����ڴ棬 ˭������ ˭����
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
				//��⵱ǰ�����Ƿ��Ѿ��򿪹����ڴ�
				if (JMEngine::game::JME_SharedMemory<T>::existMappedRegion(name))
				{
					LOGE("The shared memory with name [ %s ] had been opened", name);
					return boost::shared_ptr<T>();
				}
				//�򿪹����ڴ�
				boost::interprocess::shared_memory_object shm(boost::interprocess::open_only, name, mode);

				//�����ڴ�ӳ��
				auto mmap = new boost::interprocess::mapped_region(shm, mode);

				//��ι����ڴ�����ڴ��С �� �����С��һ�£� �����޸ģ���Сһ�� �������� ����û�б��޸ģ�
				if (sizeof(T) != mmap->get_size())
				{
					//�Ƴ��ڴ�ӳ��
					delete mmap;
					return boost::shared_ptr<T>();
				}
				//�����ڴ�ӳ��
				JMEngine::game::JME_SharedMemory<T>::saveMappedRegion(name, mmap);

				//ֱ�ӽ������ڴ� תΪ���� ���ٵ��ù��캯��
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
