#include "SignalManager.h"

void SignalManager::Emit(const std::string& signal)
{
    signals_.insert(signal);
}

bool SignalManager::Has(const std::string& signal) const
{
    return signals_.find(signal) != signals_.end();
}

void SignalManager::Clear(const std::string& signal)
{
    signals_.erase(signal);
}

void SignalManager::ClearAll()
{
    signals_.clear();
}