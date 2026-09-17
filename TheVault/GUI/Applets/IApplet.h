#pragma once

class IApplet
{
public:
	virtual void OnLeave();
	virtual void Render() = 0;
};
