#pragma once
#include <vector>
#include <string>
#include "UnsavedState.h"

class PassManager
{
private:
	std::vector<std::pair<std::string, struct Pass>> mStore;
	UnsavedState& unsavedState;

public:
	PassManager(UnsavedState& unsavedState);
	~PassManager();
	void Reset();

	int GetCount();
	bool IsPasswordText(int i);
	bool IsPasswordFile(int i);
	std::string_view GetName(int i);
	std::string_view GetPassword(int i);

	void Add(const std::string_view& name, const std::string_view& password);
	void Remove(int i);
	void Change(int i, const std::string_view& password);
	void ChangeName(int i, const std::string_view& name);
	void AddFile(const std::string_view& name, const std::wstring_view& file);
	void ChangeFile(int i, const std::wstring_view& file);
	void ExtractFile(int i, const std::wstring_view& file);

	std::string Serialize();
	bool Deserialize(const std::string_view& data);
};
