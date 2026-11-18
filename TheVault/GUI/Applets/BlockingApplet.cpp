#include "BlockingApplet.hpp"
#include "GUI/MainWindow.h"

BlockingApplet::BlockingApplet(MainWindow& mainWindow) : parent(mainWindow), isOpen(false)
{
}

BlockingApplet::~BlockingApplet()
{
}

void BlockingApplet::Open()
{
	isOpen = true;
	parent.DisableInteractions();
}

void BlockingApplet::Close()
{
	isOpen = false;
	parent.EnableInteractions();
}

void BlockingApplet::Render()
{
	if (!isOpen)
		return;
	OnRender();
}
