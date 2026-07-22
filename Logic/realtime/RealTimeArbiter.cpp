#include "RealTimeArbiter.hpp"
#include "CaptureEvent.hpp"
#include <iostream>

void RealTimeArbiter::startMotion(Motion m) {
    m.startTime = now_;
    m.piece.setState(m.state);
    motions_.push_back(ActiveMotion{m, now_ + m.durationMs});
}

void RealTimeArbiter::startJump(Position at, Piece piece, long durationMs) {
    piece.setState(PieceState::JUMP);
    jumps_.push_back(ActiveJump{at, piece, now_ + durationMs});
}

void RealTimeArbiter::startFollowUpState(const Position& at, Piece piece, PieceState state)
{
    const auto& cfg = configs_.get(piece.color(), piece.type(), state);

    long durationMs;
    if (cfg.isLoop || cfg.framesPerSecond <= 0 || cfg.frameCount <= 0)
        return;

    durationMs = static_cast<long>(cfg.frameCount) * 1000L / cfg.framesPerSecond;
    if (durationMs < 1) durationMs = 1;

    std::cout << "startFollowUpState state=" << stateToString(state)
               << " frameCount=" << cfg.frameCount
               << " fps=" << cfg.framesPerSecond
               << " durationMs=" << durationMs
               << std::endl;

    Motion m{at, at, piece, state, 0L, durationMs};
    startMotion(m);
}

std::vector<CaptureEvent> RealTimeArbiter::advanceTime(long ms, Board& board) {
    now_ += ms;
    std::vector<CaptureEvent> captured;
    std::vector<Position> capturedPositions;   // NEW

    struct PendingFollowUp { Position at; Piece piece; PieceState state; };
    std::vector<PendingFollowUp> pending;

    for (auto it = motions_.begin(); it != motions_.end(); ) {
        if (now_ >= it->arrivalAt) {
            Position from = it->motion.from;
            Position to   = it->motion.to;
            bool isStationary = (from.row == to.row && from.col == to.col);

            Piece landedPiece = it->motion.piece;

            if (!isStationary) {
                bool caughtByJumper = false;
                for (const auto& jump : jumps_) {
                    if (jump.at.row == to.row && jump.at.col == to.col &&
                        jump.piece.color() != it->motion.piece.color()) {
                        captured.push_back({it->motion.piece, jump.piece, to});
                        capturedPositions.push_back(to);   // NEW
                        caughtByJumper = true;
                        break;
                    }
                }

                board.setCell(from.row, from.col, std::nullopt);

                if (caughtByJumper) {
                    it = motions_.erase(it);
                    continue;
                }

                auto existing = board.getCell(to.row, to.col);
                if (existing.has_value())
                {
                    captured.push_back({*existing, landedPiece, to});
                    capturedPositions.push_back(to);   // NEW
                }
                int lastRow = (landedPiece.color() == Piece::Color::WHITE) ? 0 : board.rows() - 1;
                if (landedPiece.type() == Piece::Type::PAWN && to.row == lastRow) {
                    landedPiece = Piece(landedPiece.color(), Piece::Type::QUEEN);
                }
            }

            const auto& finishedCfg = configs_.get(landedPiece.color(), landedPiece.type(), it->motion.state);
            PieceState nextState = finishedCfg.nextStateWhenFinished;

            landedPiece.setState(nextState);
            board.setCell(to.row, to.col, landedPiece);

            if (nextState != PieceState::IDLE) {
                pending.push_back({to, landedPiece, nextState});
            }

            it = motions_.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = jumps_.begin(); it != jumps_.end(); ) {
        if (now_ >= it->arrivalAt) {
            const auto& finishedCfg = configs_.get(it->piece.color(), it->piece.type(), PieceState::JUMP);
            PieceState nextState = finishedCfg.nextStateWhenFinished;

            Piece landed = it->piece;
            landed.setState(nextState);

            if (nextState != PieceState::IDLE) {
                pending.push_back({it->at, landed, nextState});
            }

            it = jumps_.erase(it);
        } else {
            ++it;
        }
    }

    // NEW: purge any stale rest/jump entries left behind at positions where a capture just happened,
    // so a dead piece's leftover animation state can never resurrect it onto the board.
    if (!capturedPositions.empty())
    {
        auto isCapturedPos = [&](const Position& p) {
            for (const auto& cp : capturedPositions)
                if (cp.row == p.row && cp.col == p.col) return true;
            return false;
        };

        motions_.erase(std::remove_if(motions_.begin(), motions_.end(),
            [&](const ActiveMotion& am) {
                bool stationary = (am.motion.from.row == am.motion.to.row &&
                                    am.motion.from.col == am.motion.to.col);
                return stationary && isCapturedPos(am.motion.to);
            }), motions_.end());

        jumps_.erase(std::remove_if(jumps_.begin(), jumps_.end(),
            [&](const ActiveJump& aj) { return isCapturedPos(aj.at); }), jumps_.end());
    }

    for (const auto& p : pending) {
        startFollowUpState(p.at, p.piece, p.state);
    }

    return captured;
}

bool RealTimeArbiter::hasActiveMotionFrom(const Position& from) const {
    for (const auto& am : motions_)
        if (am.motion.from.row == from.row && am.motion.from.col == from.col) return true;
    return false;
}

bool RealTimeArbiter::conflictsWithActiveMotion(const Position& from, const Position& to) const {
    for (const auto& am : motions_) {
        bool isStationary = (am.motion.from.row == am.motion.to.row &&
                              am.motion.from.col == am.motion.to.col);

        if (isStationary)
            continue;   // כלי במנוחה לא נחשב "תנועה מתחרה"

        if (am.motion.from.row == from.row && am.motion.from.col == from.col) return true;
        if (am.motion.to.row == to.row && am.motion.to.col == to.col) return true;
        if (am.motion.from.row == to.row && am.motion.from.col == to.col) return true;
    }
    return false;
}

bool RealTimeArbiter::isJumpingAt(const Position& pos) const {
    for (const auto& j : jumps_)
        if (j.at.row == pos.row && j.at.col == pos.col) return true;
    return false;
}

std::optional<RenderPosition>
RealTimeArbiter::currentPositionOf(const Position& from) const
{
    for (const auto& active : motions_) {
        if (active.motion.from.row == from.row && active.motion.from.col == from.col) {
            double progress = (active.motion.durationMs > 0)
            ? double(now_ - active.motion.startTime) / active.motion.durationMs
            : 1.0;   // אם משום מה durationMs==0, נחשיב כמסיים מיד ולא ניצור NaN
            if (progress < 0) progress = 0;
            if (progress > 1) progress = 1;

            double row = active.motion.from.row +
                (active.motion.to.row - active.motion.from.row) * progress;
            double col = active.motion.from.col +
                (active.motion.to.col - active.motion.from.col) * progress;

            return RenderPosition{row, col};
        }
    }
    return std::nullopt;
}

long RealTimeArbiter::currentTime() const { return now_; }

std::optional<PieceState> RealTimeArbiter::currentStateOf(const Position& from) const
{
    for (const auto& active : motions_)
    {
        if (active.motion.from.row == from.row && active.motion.from.col == from.col)
            return active.motion.state;
    }

    for (const auto& j : jumps_)
    {
        if (j.at.row == from.row && j.at.col == from.col)
            return PieceState::JUMP;
    }

    return std::nullopt;
}
std::optional<long> RealTimeArbiter::stateStartTimeOf(const Position& from) const
{
    for (const auto& active : motions_)
    {
        if (active.motion.from.row == from.row && active.motion.from.col == from.col)
            return active.motion.startTime;
    }
    return std::nullopt;
}
std::optional<long> RealTimeArbiter::stateDurationOf(const Position& from) const
{
    for (const auto& active : motions_)
    {
        if (active.motion.from.row == from.row && active.motion.from.col == from.col)
            return active.motion.durationMs;
    }
    return std::nullopt;
}