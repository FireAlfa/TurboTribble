#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL2.lib" )
#pragma comment( lib, "SDL2main.lib" )

enum class MainStates
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main(int argc, char ** argv)
{
	TTLOG("++++++++++ Starting %s Engine ++++++++++", TITLE);

	int mainReturn = EXIT_FAILURE;
	MainStates state = MainStates::MAIN_CREATION;
	Application* app = NULL;

	while (state != MainStates::MAIN_EXIT)
	{
		switch (state)
		{
		case MainStates::MAIN_CREATION:

			TTLOG("+++++ Application Creation +++++");
			app = new Application();
			state = MainStates::MAIN_START;
			break;

		case MainStates::MAIN_START:

			TTLOG("+++++ Application Init +++++");
			if (app->Init() == false)
			{
				TTLOG("##### Application Init exits with ERROR #####");
				state = MainStates::MAIN_EXIT;
			}
			else
			{
				state = MainStates::MAIN_UPDATE;
				TTLOG("+++++ Application Update +++++");
			}

			break;

		case MainStates::MAIN_UPDATE:
		{
			UpdateStatus update_return = app->Update();

			if (update_return == UpdateStatus::UPDATE_ERROR)
			{
				TTLOG("##### Application Update exits with ERROR #####");
				state = MainStates::MAIN_EXIT;
			}

			if (update_return == UpdateStatus::UPDATE_STOP)
				state = MainStates::MAIN_FINISH;
		}
			break;

		case MainStates::MAIN_FINISH:

			TTLOG("+++++ Application CleanUp +++++");
			if (app->CleanUp() == false)
			{
				TTLOG("##### Application CleanUp exits with ERROR #####");
			}
			else
				mainReturn = EXIT_SUCCESS;

			state = MainStates::MAIN_EXIT;

			break;

		}
	}

	delete app;
	TTLOG("++++++++++ Exiting %s Engine ++++++++++\n", TITLE);
	return mainReturn;
}