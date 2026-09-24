#pragma once
#include <vector>
#include <string>
#include "UnsavedState.h"

class PassManager
{
private:
	std::vector<std::pair<std::string, struct Pass>> mStore;
	UnsavedState& unsavedState;

	bool CheckBounds(int i);

public:
	PassManager(UnsavedState& unsavedState);
	~PassManager();
	void Reset();

	int GetCount();
	bool IsText(int i);
	bool IsFile(int i);

	int Add(const std::string_view& name);
	void Remove(int i);

	void SetName(int i, const std::string_view& name);
	std::string_view GetName(int i);

	void AddText(const std::string_view& name, const std::string_view& password);
	void SetText(int i, const std::string_view& password);
	std::string_view GetText(int i);

	bool AddFile(const std::string_view& name, const std::wstring_view& file);
	bool SetFile(int i, const std::wstring_view& file);
	bool ExtractFile(int i, const std::wstring_view& file);

	std::string Serialize();
	bool Deserialize(const std::string_view& data);
};
