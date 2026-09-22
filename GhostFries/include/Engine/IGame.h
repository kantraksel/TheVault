#pragma once

class IGame
{
	public:
		virtual bool OnInitialize() = 0;
		virtual bool OnShutdown() = 0;

		//virtual bool OnApplicationExit() = 0;
};
