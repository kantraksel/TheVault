#pragma once

class IApplet
{
public:
	virtual void OnEnter();
	virtual void OnLeave();
	virtual void Render() = 0;
};
