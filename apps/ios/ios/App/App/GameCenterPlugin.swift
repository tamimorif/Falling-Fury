import Capacitor
import GameKit

@objc(GameCenterPlugin)
public class GameCenterPlugin: CAPPlugin, CAPBridgedPlugin {
    public let identifier = "GameCenterPlugin"
    public let jsName = "GameCenter"
    public let pluginMethods: [CAPPluginMethod] = [
        CAPPluginMethod(name: "getLeaderboard", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "submitScore", returnType: CAPPluginReturnPromise)
    ]

    private let leaderboardID = "com.tamimorif.fallingfury.highscore"
    private var pendingLeaderboardCall: CAPPluginCall?

    @objc public func getLeaderboard(_ call: CAPPluginCall) {
        guard GKLocalPlayer.local.isAuthenticated else {
            pendingLeaderboardCall = call
            GKLocalPlayer.local.authenticateHandler = { [weak self] viewController, _ in
                DispatchQueue.main.async {
                    guard let self else { return }
                    if let viewController {
                        self.bridge?.viewController?.present(viewController, animated: true)
                    } else if GKLocalPlayer.local.isAuthenticated {
                        self.loadLeaderboard()
                    } else {
                        self.pendingLeaderboardCall?.resolve(["available": false])
                        self.pendingLeaderboardCall = nil
                    }
                }
            }
            return
        }
        loadLeaderboard(call: call)
    }

    @objc public func submitScore(_ call: CAPPluginCall) {
        guard GKLocalPlayer.local.isAuthenticated else {
            call.resolve(["submitted": false])
            return
        }
        let score = max(0, call.getInt("score") ?? 0)
        guard score > 0 else {
            call.resolve(["submitted": false])
            return
        }
        GKLeaderboard.submitScore(
            score,
            context: 0,
            player: GKLocalPlayer.local,
            leaderboardIDs: [leaderboardID]
        ) { error in
            DispatchQueue.main.async {
                call.resolve(["submitted": error == nil])
            }
        }
    }

    private func loadLeaderboard() {
        guard let call = pendingLeaderboardCall else { return }
        loadLeaderboard(call: call)
    }

    private func loadLeaderboard(call: CAPPluginCall) {
        GKLeaderboard.loadLeaderboards(IDs: [leaderboardID]) { [weak self] leaderboards, error in
            guard let self else { return }
            guard error == nil, let leaderboard = leaderboards?.first else {
                DispatchQueue.main.async {
                    call.resolve(["available": false])
                    self.clearPending(call)
                }
                return
            }
            leaderboard.loadEntries(
                for: .global,
                timeScope: .allTime,
                range: NSRange(location: 1, length: 10)
            ) { localEntry, entries, _, loadError in
                DispatchQueue.main.async {
                    guard loadError == nil else {
                        call.resolve(["available": false])
                        self.clearPending(call)
                        return
                    }
                    let rows = (entries ?? []).map { entry in
                        [
                            "rank": entry.rank,
                            "score": entry.score,
                            "name": entry.player.displayName,
                            "isPlayer": entry.player.gamePlayerID == GKLocalPlayer.local.gamePlayerID
                        ] as [String: Any]
                    }
                    var result: [String: Any] = [
                        "available": true,
                        "playerName": GKLocalPlayer.local.displayName,
                        "entries": rows
                    ]
                    if let localEntry {
                        result["playerRank"] = localEntry.rank
                        result["playerScore"] = localEntry.score
                    }
                    call.resolve(result)
                    self.clearPending(call)
                }
            }
        }
    }

    private func clearPending(_ call: CAPPluginCall) {
        if pendingLeaderboardCall === call {
            pendingLeaderboardCall = nil
        }
    }
}
