#include "SignalManager.h"

// 通知を送る
void SignalManager::Emit(const std::string& signal)
{
    signals_.insert(signal);
}

// 指定した通知が存在するか確認する
bool SignalManager::Has(const std::string& signal) const
{
    return signals_.find(signal) != signals_.end();
}

// 指定した通知を消す
void SignalManager::Clear(const std::string& signal)
{
    signals_.erase(signal);
}

// すべての通知を消す
void SignalManager::ClearAll()
{
    signals_.clear();
}