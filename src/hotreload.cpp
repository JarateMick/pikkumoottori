


static bool loadGameDll(ApplicationFunctions* app, const char* dllName);
void initializeApplication(ApplicationFunctions* app, const char* name, 
	const char* initF, const char* updateF, const char* drawF)
{
	strcpy(app->initName, initF);
	strcpy(app->updateName, updateF);
	strcpy(app->drawName, drawF);

	char appName[64];
	sprintf(appName, "%s.dll", name);

	loadGameDll(app, appName);
	
	ASSERT(strlen(name) + 4 < ArrayCount(ApplicationFunctions::name));
	char* pos = strcpy(app->name, name);
	strcpy(pos + strlen(name), ".dll");

}

void copyFileOs(const char* infile, const char* outfile)
{
#ifdef _WIN32 //|| _WIN64
	//  Windows
	CopyFileA(infile, outfile, false);
#elif __APPLE__
	//  OSX
	copyfile(infile, outfile, NULL, COPYFILE_DATA);
#elif __linux
	//  Linux
#if 0 
	struct stat stat_buf;
	int in_fd, out_fd;
	offset_t offset = 0;

	in_fd = open(infile, O_RDONLY);
	fstat(in_fd, &stat_buf);
	out_fd = open(outfile, O_WRONLY | O_CREAT, stat_buf.st_mode);

	sendfile(out_fd, in_fd, &offset, stat_buf.st_size);

	close(out_fd);
	close(in_fd);
#endif
#endif
}

static void unloadGameDll(ApplicationFunctions* app)
{
	SDL_UnloadObject(app->handle);
	app->gameInitPtr = app->gameUpdatePtr = app->gameDrawPtr = 0;
}

static time_t getWriteTime(const char* filename)
{
	struct stat stats;
	stat(filename, &stats);

	time_t mtime = stats.st_mtime; /* seconds since the epoch */
	return mtime;
}

static bool loadGameDll(ApplicationFunctions* app, const char* dllName)
{
#if _WIN32
	WIN32_FILE_ATTRIBUTE_DATA unused;
	if (!GetFileAttributesEx("lock.tmp", GetFileExInfoStandard, &unused))
	{
#else
		{
#endif

			unloadGameDll(app);

			static char buffer[256];
			sprintf(buffer, "%s", dllName);
#if _WIN32
			buffer[strlen(dllName) - 4] = '\0';
			strcat(buffer, "_temp.dll");
#else
			buffer[strlen(dllName) - 3] = '\0';
			strcat(buffer, "_temp.so");
#endif

#if _WIN32
			bool success = CopyFileEx(dllName, buffer, NULL, NULL, false, COPY_FILE_NO_BUFFERING);
			// CopyFileA(dllName, buffer, false);
#endif
			// void* handle = SDL_LoadObject(buffer); // temp
			// void* handle = SDL_LoadObject(buffer); // temp

			// handle = dlopen(buffer, RTLD_LAZY);
#if _LINUX
			while (!copyFile(dllName, buffer)) {}

			void* handle = dlopen("./libgame_temp.so", RTLD_LAZY);
			LOGI("handle: %p %i name: %s\n", handle, handle, buffer);
			LOGI("error %s\n", dlerror());
			START_TIMING2()
#else
			void* handle = SDL_LoadObject(buffer); // temp
			printf("%s \n", SDL_GetError());
#endif
			if (handle)
			{
				app->handle = handle;
				LOGI("dynamic library load success! \n");


				update_Game* gameUpdate = (update_Game*)SDL_LoadFunction(handle, app->updateName);

				if (gameUpdate)
				{
					app->gameUpdatePtr = gameUpdate;
					printf("success \n");
				}
				else
				{
					LOGI("counld not load game update!\n");
				}

				draw_Game*   gameDraw = (draw_Game*)SDL_LoadFunction(handle, app->drawName);
				if (gameDraw)
				{
					app->gameDrawPtr = gameDraw;
				}
				else
				{
					LOGI("counld not load game draw!\n");
				}

				init_game*   gameInit = (init_game*)SDL_LoadFunction(handle, app->initName);
				if (gameInit)
				{
					app->gameInitPtr = gameInit;
				}
				else
				{
					LOGI("could't load game init! \n");
				}

				app->lastWriteTime = getWriteTime(buffer);
				
			
			}
			return true;
	}
		return false;
}

void callUpdate(EngineContext* engine, ApplicationFunctions* app)
{
	if (app->gameUpdatePtr)
	{
		app->gameUpdatePtr(engine, &app->memory);
	}
}

void callRender(EngineContext* engine, ApplicationFunctions* app)
{
	if (app->gameDrawPtr)
	{
		app->gameDrawPtr(engine, &app->memory);
	}
}

void callInit(EngineContext* engine, ApplicationFunctions* app)
{
	if (app->gameInitPtr)
	{
		app->gameInitPtr(engine, &app->memory);
	}
}

bool update(ApplicationFunctions* app)
{
	size_t writeTime = getWriteTime(app->name);
	if (writeTime != app->lastWriteTime)
	{
		unloadGameDll(app);
		loadGameDll(app, app->name);
		return true;
	}
	return false;
}

