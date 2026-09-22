#pragma once

class UniqueObject
{
protected:
	UniqueObject() = default;
	UniqueObject(UniqueObject&) = delete;
	UniqueObject& operator=(UniqueObject&) = delete;
};

class InsuredObject
{
protected:
	InsuredObject() = default;
	InsuredObject(InsuredObject&&) = delete;
	InsuredObject& operator=(InsuredObject&&) = delete;
};

class LockedObject
{
protected:
	LockedObject() = default;
	LockedObject(LockedObject&) = delete;
	LockedObject& operator=(LockedObject&) = delete;
	LockedObject(LockedObject&&) = delete;
	LockedObject& operator=(LockedObject&&) = delete;
};
