#include "ClientAnimationState.hpp"
#include <algorithm>

void ClientAnimationState::startMotion(const Position& from, const Position& to, long durationMs)
{
    activeMotions_[from] = ActiveMotion{ from, to, currentTime(), durationMs };
}

void ClientAnimationState::updateFromMessage(const nlohmann::json& msg)
{
    std::map<Position, PieceRenderState> updated;
    long localNow = currentTime();

    for (const auto& p : msg["pieces"])
    {
        Position pos{ p["row"].get<int>(), p["col"].get<int>() };
        PieceRenderState info;

        if (p.contains("renderRow") && p.contains("renderCol"))
            info.renderPos = RenderPosition{ p["renderRow"].get<double>(),
                                              p["renderCol"].get<double>() };
        if (p.contains("state"))
            info.state = static_cast<PieceState>(p["state"].get<int>());
        if (p.contains("stateDuration"))
            info.stateDuration = p["stateDuration"].get<long>();
        if (p.contains("stateElapsed"))
            info.stateStartTime = localNow - p["stateElapsed"].get<long>();

        updated[pos] = info;
    }

    pieces_ = std::move(updated);
}

std::optional<RenderPosition> ClientAnimationState::currentPositionOf(const Position& pos) const
{
     auto it = activeMotions_.find(pos);
    if (it != activeMotions_.end())
    {
        const ActiveMotion& m = it->second;
        long elapsed = currentTime() - m.startTime;

        if (elapsed >= m.durationMs)
        {
            activeMotions_.erase(it);
        }
        else
        {
            double progress = std::clamp(double(elapsed) / double(m.durationMs), 0.0, 1.0);
            RenderPosition rp;
            rp.row = m.from.row + (m.to.row - m.from.row) * progress;
            rp.col = m.from.col + (m.to.col - m.from.col) * progress;
            return rp;
        }
    }

    auto pit = pieces_.find(pos);
    return pit != pieces_.end() ? pit->second.renderPos : std::nullopt;
}

std::optional<PieceState> ClientAnimationState::currentStateOf(const Position& pos) const
{
    if (activeMotions_.count(pos))
        return PieceState::MOVE;

    auto it = pieces_.find(pos);
    return it != pieces_.end() ? it->second.state : std::nullopt;
}

std::optional<long> ClientAnimationState::stateStartTimeOf(const Position& pos) const
{
    auto mit = activeMotions_.find(pos);
    if (mit != activeMotions_.end())
        return mit->second.startTime;

    auto it = pieces_.find(pos);
    return it != pieces_.end() ? it->second.stateStartTime : std::nullopt;

}

std::optional<long> ClientAnimationState::stateDurationOf(const Position& pos) const
{
     auto mit = activeMotions_.find(pos);
    if (mit != activeMotions_.end())
        return mit->second.startTime;

    auto it = pieces_.find(pos);
    return it != pieces_.end() ? it->second.stateStartTime : std::nullopt;

}

long ClientAnimationState::currentTime() const
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}