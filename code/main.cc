//------------------------------------------------------------------------------
//  testmem/main.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "system/appentry.h"
#include "demoapp.h"
#include "app/application.h"
#include "util/commandlineargs.h"

using namespace Core;

ImplementNebulaApplication();

void
NebulaMain(const Util::CommandLineArgs& args)
{
	Demo::DemoApplication app;
	
	app.SetCompanyName("Nebula");
	app.SetAppTitle("Demo Application");
	app.SetCmdLineArgs(args);

    if (app.Open())
    {
        app.Run();
        app.Close();
    }

	Core::SysFunc::Exit(0);
}
