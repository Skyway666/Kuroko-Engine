#include <stdlib.h>
#include "Application.h"
#include "Globals.h"
#include "Applog.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/lib/SDL2.lib" )
#pragma comment( lib, "SDL/lib/SDL2main.lib" )

#include <stdlib.h>  
//#include <vld.h>

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App;
AppLog* app_log;

int main(int argc, char ** argv)
{
	app_log = new AppLog();
	app_log->AddLog("Starting engine '%s'...", TITLE);

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			app_log->AddLog("-------------- Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			app_log->AddLog("-------------- Application Init --------------\n");
			if (App->Init() == false)
			{
				app_log->AddLog("Application Init exits with ERROR\n");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				app_log->AddLog("\n -------------- Application Update --------------\n");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				app_log->AddLog("Application Update exits with ERROR\n");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			app_log->AddLog("-------------- Application CleanUp --------------\n");
			if (App->CleanUp() == false)
			{
				app_log->AddLog("Application CleanUp exits with ERROR\n");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	app_log->AddLog("Exiting game '%s'...\n", TITLE);
	delete app_log;

	_CrtDumpMemoryLeaks();

	return main_return;
}