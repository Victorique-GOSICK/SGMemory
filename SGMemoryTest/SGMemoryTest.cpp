// SGMemory.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Object/SGObjectManager.h"
#include "Container/SGContainers.h"


//////////////////////////////////////////////////////////////////////////

class SGActor : public SGObject
{
    SG_OBJECT_TYPE_DECL(SGActor, SGObject);
protected:
    SGFixedString ActorName;

protected:
	void MakeSnapshot() const override
	{
        ActorName.MakeSnapshot();
	}


public:
    void Create(const char* InName)
    {
        ActorName = InName;
    }
public:
    virtual void Tick(float InDeltaTime)
    {
        printf("[%p] SGActor<%s>::Tick(%f)\n", this, *ActorName, InDeltaTime);
    }
    
};
SG_OBJECT_TYPE_IMPL(SGActor);

//////////////////////////////////////////////////////////////////////////
class SGPlayer : public SGActor
{
	SG_OBJECT_TYPE_DECL(SGPlayer, SGActor);

public:
	virtual void Tick(float InDeltaTime)
	{
		printf("[%p] %s<%s>::Tick(%f)\n", this, GetTypeInfo()->Name, *ActorName, InDeltaTime);
        SGActor::Tick(InDeltaTime);
	}
};
SG_OBJECT_TYPE_IMPL(SGPlayer);

//////////////////////////////////////////////////////////////////////////
class SGEnemy: public SGActor
{
	SG_OBJECT_TYPE_DECL(SGEnemy, SGActor);

public:
	virtual void Tick(float InDeltaTime)
	{
		printf("[%p] %s<%s>::Tick(%f)\n", this, GetTypeInfo()->Name, *ActorName, InDeltaTime);
		SGActor::Tick(InDeltaTime);
	}
};
SG_OBJECT_TYPE_IMPL(SGEnemy);

//////////////////////////////////////////////////////////////////////////
class SGWorld : public SGObject
{
	SG_OBJECT_TYPE_DECL(SGWorld, SGObject);
private:
    SGPlayer* Actor1 = nullptr;
    SGEnemy* Actor2 = nullptr;

protected:
    void MakeSnapshot() const override
    {
        MarkPtrAddr((void**)&Actor1);
        MarkPtrAddr((void**)&Actor2);
    }

public:
    void Create()
    {
        Actor1 = NewObject<SGPlayer>();
        Actor1->Create("Slight");
		Actor2 = NewObject<SGEnemy>();
		Actor2->Create("Cold");
    }

    void KillEnemy()
    {
        printf("[%p] SGWorld::KillEnemy()\n", this);
        FreeObject(Actor2);
        Actor2 = nullptr;
    }

	void NewEnemy()
	{
		printf("[%p] SGWorld::NewEnemy()\n", this);
		Actor2 = NewObject<SGEnemy>();
		Actor2->Create("NewEnemy");
		Actor2 = NewObject<SGEnemy>();
		Actor2->Create("NewEnemy");
	}

	virtual void Tick(float InDeltaTime)
	{
		printf("[%p] SGWorld::Tick(%f)\n", this, InDeltaTime);
        Actor1->Tick(InDeltaTime);
        if (Actor2)
        {
            Actor2->Tick(InDeltaTime);
        }
	}
};
SG_OBJECT_TYPE_IMPL(SGWorld);

//////////////////////////////////////////////////////////////////////////


SGHandle CreateWorld()
{
	SGWorld* World = NewObject<SGWorld>();
	World->Create();
	return World->GetHandle();
}


int main()
{
    SGHandle WorldHandle;

    
    {
        //����һ��Chunk
		GDefaultMemoryManager = new SGMemoryManager();
        printf("MemoryChunk = %p\n", GDefaultMemoryManager->GetMemoryChunk()->GetBasePtr());

        //������Ϸ����
        WorldHandle = CreateWorld();

        //ִ���߼�
		SGHandlePtr<SGWorld> World = WorldHandle;
		World->Tick(0.33f);
        //World->KillEnemy();
        //World->Tick(0.33f);
        //World->NewEnemy();
        //World->Tick(0.33f);
    }
    
    //��������
	SGMemorySnapshot MemSnapshot = GDefaultMemoryManager->MakeSnapshot();
	SGObjectSnapshot ObjSnapshot = SGObjectManager::Get().MakeSnapshot();


	//�����ڴ�
	{
		delete GDefaultMemoryManager;
		new SGMemoryManager();
	}

    {
        //�����µ�Chunk
        GDefaultMemoryManager = new SGMemoryManager();
        printf("MemoryChunk = %p\n", GDefaultMemoryManager->GetMemoryChunk()->GetBasePtr());

        //�ָ�����
        GDefaultMemoryManager->ResumeSnapshot(MemSnapshot);
        SGObjectManager::Get().ResumeSnapshot(ObjSnapshot);

        //ֱ��ִ���߼�
        SGHandlePtr<SGWorld> World = WorldHandle;
        World->Tick(0.33f);
		World->KillEnemy();
		World->Tick(0.33f);
		World->NewEnemy();
		World->Tick(0.33f);
    }

    std::cout << "Hello World!\n";
}

