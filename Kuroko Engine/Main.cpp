#include <stdlib.h>
#include "Application.h"
#include "Globals.h"
#include "ModuleImGUI.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/lib/SDL2.lib" )
#pragma comment( lib, "SDL/lib/SDL2main.lib" )

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App;

int main(int argc, char ** argv)
{
	APPLOG("Starting engine '%s'...", TITLE);

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			APPLOG("-------------- Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			App->gui->getLog()->AddLog("-------------- Application Init --------------\n");
			if (App->Init() == false)
			{
				App->gui->getLog()->AddLog("Application Init exits with ERROR\n");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				App->gui->getLog()->AddLog("-------------- Application Update --------------\n");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				App->gui->getLog()->AddLog("Application Update exits with ERROR\n");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			App->gui->getLog()->AddLog("-------------- Application CleanUp --------------\n");
			if (App->CleanUp() == false)
			{
				App->gui->getLog()->AddLog("Application CleanUp exits with ERROR\n");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	APPLOG("Exiting game '%s'...\n", TITLE);
	return main_return;
}