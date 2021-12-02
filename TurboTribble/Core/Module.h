#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"

#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "rapidjson-1.1.0/include/rapidjson/document.h"



class Application;

typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> JSONWriter;
typedef rapidjson::Value JSONReader;

class Module
{
private :

	bool enabled;

public:

	Application* app;

	Module(Application* parent, bool startEnabled = true) : app(parent)
	{}

	virtual ~Module()
	{}
	
	virtual void OnLoad(const JSONReader& reader)
	{		
	}
	
	virtual void OnSave(JSONWriter& writer) const
	{

	}

	virtual bool Init()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual UpdateStatus PreUpdate(float dt)
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}

	virtual UpdateStatus Update(float dt)
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}

	virtual UpdateStatus PostUpdate(float dt)
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void OnGui() {}
};

#endif // !__MODULE_H__