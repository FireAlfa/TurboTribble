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

Application* app = NULL;

int main(int argc, char** argv)
{
	

	int mainReturn = EXIT_FAILURE;
	MainStates state = MainStates::MAIN_CREATION;
	

	while (state != MainStates::MAIN_EXIT)
	{
		switch (state)
		{
		case MainStates::MAIN_CREATION:
		{
			app = new Application();
			TTLOG("~~~~~~~~~~ Starting %s Engine ~~~~~~~~~~\n", TITLE);
			TTLOG("++++++++ Application Creation ++++++++\n");
			state = MainStates::MAIN_START;

			break;
		}

		case MainStates::MAIN_START:
		{
			TTLOG("++++++++ Application Init ++++++++\n");
			if (app->Init() == false)
			{
				TTLOG("######## Application Init exits with ERROR ########\n");
				state = MainStates::MAIN_EXIT;
			}
			else
			{
				state = MainStates::MAIN_UPDATE;
				TTLOG("++++++++ Application Update ++++++++\n");
			}

			break;
		}

		case MainStates::MAIN_UPDATE:
		{
			UpdateStatus updateReturn = app->Update();

			if (updateReturn == UpdateStatus::UPDATE_ERROR)
			{
				TTLOG("######## Application Update exits with ERROR ########\n");
				state = MainStates::MAIN_EXIT;
			}

			if (updateReturn == UpdateStatus::UPDATE_STOP)
				state = MainStates::MAIN_FINISH;

			break;
		}

		case MainStates::MAIN_FINISH:
		{
			TTLOG("++++++++ Application CleanUp ++++++++\n");
			if (app->CleanUp() == false)
			{
				TTLOG("######## Application CleanUp exits with ERROR ########\n");
			}
			else
				mainReturn = EXIT_SUCCESS;

			state = MainStates::MAIN_EXIT;

			break;

		}
		}
	}

	TTLOG("~~~~~~~~~~ Exiting %s Engine ~~~~~~~~~~\n", TITLE);
	delete app;
	return mainReturn;
}