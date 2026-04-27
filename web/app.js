const ICON_GAME = `<svg class="ti" viewBox="0 0 24 24" aria-hidden="true"><path d="M6 12h4m-2 -2v4"/><path d="M15 11l.01 0"/><path d="M18 13l.01 0"/><path d="M5 8h14a2 2 0 0 1 2 2v4a4 4 0 0 1 -4 4a4 4 0 0 1 -3 -1.35a3 3 0 0 0 -4 0a4 4 0 0 1 -3 1.35a4 4 0 0 1 -4 -4v-4a2 2 0 0 1 2 -2z"/></svg>`;
const ICON_TROPHY = `<svg class="ti" viewBox="0 0 24 24" aria-hidden="true"><path d="M8 21h8"/><path d="M12 17v4"/><path d="M7 4h10v4a5 5 0 0 1 -10 0v-4z"/><path d="M5 5h-2v2a3 3 0 0 0 4 2.83"/><path d="M19 5h2v2a3 3 0 0 1 -4 2.83"/></svg>`;

const GAMES = {
  Valorant: [
    "Iron",
    "Bronze",
    "Silver",
    "Gold",
    "Platinum",
    "Diamond",
    "Immortal",
    "Radiant",
  ],
  "League of Legends": [
    "Iron",
    "Bronze",
    "Silver",
    "Gold",
    "Platinum",
    "Diamond",
    "Master",
    "Grandmaster",
    "Challenger",
  ],
  "Counter-Strike 2": [
    "Silver I",
    "Silver II",
    "Silver III",
    "Silver IV",
    "Silver V",
    "Silver Elite",
    "Silver Elite Master",
    "Gold Nova I",
    "Gold Nova II",
    "Gold Nova III",
    "Gold Nova Master",
    "Master Guardian I",
    "Master Guardian II",
    "Master Guardian Elite",
    "Legendary Eagle",
    "Legendary Eagle Master",
    "Supreme Master First Class",
  ],
  "Dota 2": [
    "Herald",
    "Guardian",
    "Crusader",
    "Archon",
    "Legend",
    "Ancient",
    "Divine",
    "Immortal",
  ],
  "Street Fighter 6": [
    "Bronze",
    "Silver",
    "Gold",
    "Platinum",
    "Diamond",
    "Master",
    "Grandmaster",
  ],
  "Tekken 8": [
    "Beginner",
    "Fighter",
    "Competitor",
    "Contender",
    "Champion",
    "Divine Ruler",
  ],
  "StarCraft II": [
    "Bronze",
    "Silver",
    "Gold",
    "Platinum",
    "Diamond",
    "Master",
    "Grandmaster",
  ],
  Fortnite: [
    "Bronze",
    "Silver",
    "Gold",
    "Platinum",
    "Diamond",
    "Elite",
    "Master",
    "Hyper",
  ],
};

let state = { players: [], queue: [], dashboard: null, selectedPlayer: null };
const $ = (s) => document.querySelector(s);
function body(data) {
  return new URLSearchParams(data).toString();
}
async function get(p) {
  return await (await fetch(p)).json();
}
async function post(p, d = {}) {
  return await (
    await fetch(p, {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: body(d),
    })
  ).json();
}
function toast(m, e = false) {
  const t = $("#toast");
  t.textContent = m;
  t.className = "toast show" + (e ? " error" : "");
  setTimeout(() => (t.className = "toast"), 2600);
}
function cls(r) {
  return r >= 70 ? "high" : r >= 40 ? "mid" : "";
}
function esc(v) {
  return String(v ?? "").replace(
    /[&<>"']/g,
    (c) =>
      ({
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        '"': "&quot;",
        "'": "&#039;",
      })[c],
  );
}
async function refresh() {
  const [d, p, q, h, w, m] = await Promise.all([
    get("/api/dashboard"),
    get("/api/players"),
    get("/api/queue"),
    get("/api/history"),
    get("/api/winners"),
    get("/api/match"),
  ]);
  state.dashboard = d;
  state.players = p;
  state.queue = q;
  dash(d);
  players(p);
  queue(q, d);
  match(m);
  history(h);
  winners(w);
}
function dash(d) {
  $("#topPlayerName").textContent = d.topPlayer
    ? d.topPlayer.name
    : "No players";
  $("#topPlayerRecord").textContent = d.topPlayer
    ? `${d.topPlayer.wins}W / ${d.topPlayer.losses}L`
    : "0W / 0L";
  $("#bestWinRate").textContent = d.bestWinRate
    ? `${d.bestWinRate.winRate.toFixed(1)}%`
    : "0.0%";
  $("#bestWinRateName").textContent = d.bestWinRate
    ? d.bestWinRate.name
    : "No ranked player";
  $("#queueCount").textContent = `${d.queueCount} players`;
  $("#activeMatchTitle").textContent = d.hasActiveMatch
    ? "LIVE"
    : d.nextMatch
      ? "Next"
      : "Waiting";
  $("#activeMatchInfo").textContent = d.hasActiveMatch
    ? `${d.activeMatch.player1} vs ${d.activeMatch.player2}`
    : d.nextMatch
      ? `${d.nextMatch.player1.name} vs ${d.nextMatch.player2.name}`
      : "No active match";
  versus("#nextMatchPreview", d.nextMatch);
  $("#totalMatches").textContent = d.totalMatches || 0;
  $("#activePlayers").textContent = state.players.length;
  const gameCounts = {};
  state.players.forEach((p) => {
    gameCounts[p.gameName] = (gameCounts[p.gameName] || 0) + 1;
  });
  const mostPlayed = Object.keys(gameCounts).reduce(
    (a, b) => (gameCounts[a] > gameCounts[b] ? a : b),
    "",
  );
  $("#mostPlayedGame").textContent = mostPlayed || "—";
  $("#matchesInQueue").textContent = d.queueCount;
  if (d.hasActiveMatch) {
    $("#liveStatus").classList.add("active");
    $("#liveStatus").style.opacity = "1";
  } else {
    $("#liveStatus").classList.remove("active");
    $("#liveStatus").style.opacity = "0.3";
  }
  $("#queueStatus").style.opacity = d.queueCount > 0 ? "1" : "0.3";
}
function versus(sel, n) {
  $(sel).innerHTML = n
    ? `<div class="fighter">${esc(n.player1.name)}</div><strong>VS</strong><div class="fighter">${esc(n.player2.name)}</div>`
    : `<div class="fighter">Waiting</div><strong>VS</strong><div class="fighter">Waiting</div>`;
}
function players(ps) {
  const list = $("#playersList");
  if (!ps.length) {
    list.innerHTML = `<div class="empty-state">No players found.</div>`;
    profile(null);
    return;
  }
  const sorted = [...ps].sort((a, b) =>
    b.wins === a.wins ? a.losses - b.losses : b.wins - a.wins,
  );
  if (
    !state.selectedPlayer ||
    !sorted.some((p) => p.id === state.selectedPlayer.id)
  )
    state.selectedPlayer = sorted[0];
  list.innerHTML = sorted
    .map(
      (p) =>
        `<div class="roster-row ${state.selectedPlayer?.id === p.id ? "selected" : ""}"><div class="player-id">${esc(p.id)}</div><div class="player-name">${esc(p.name)}</div><div class="badge icon-badge">${ICON_GAME}${esc(p.gameName)}</div><div class="badge icon-badge">${ICON_TROPHY}${esc(p.rank)}</div><div class="badge rate-badge ${cls(p.winRate)}">${p.winRate.toFixed(1)}%</div><button class="btn primary small view-player" data-id="${esc(p.id)}">View</button></div>`,
    )
    .join("");
  document.querySelectorAll(".view-player").forEach(
    (b) =>
      (b.onclick = () => {
        state.selectedPlayer = state.players.find((p) => p.id === b.dataset.id);
        players(state.players);
        profile(state.selectedPlayer);
      }),
  );
  profile(state.selectedPlayer);
}
function profile(p) {
  if (!p) {
    $("#profileName").textContent = "Select a player";
    $("#profileId").textContent = "ID —";
    $("#profileData").innerHTML =
      `<div>GAME —</div><div>RANK —</div><div>AGE —</div><div>RECORD 0W / 0L</div><div class="rate">WIN RATE 0.0%</div>`;
    return;
  }
  $("#profileName").textContent = p.name;
  $("#profileId").textContent = `ID ${p.id}`;
  $("#profileData").innerHTML =
    `<div>GAME ${esc(p.gameName)}</div><div>RANK ${esc(p.rank)}</div><div>AGE ${p.age}</div><div>RECORD ${p.wins}W / ${p.losses}L</div><div class="rate ${cls(p.winRate)}">WIN RATE ${p.winRate.toFixed(1)}%</div>`;
}
function queue(q, d) {
  versus("#queueVersus", d.nextMatch);
  $("#queueList").innerHTML = q.length
    ? q
        .map(
          (p, i) =>
            `<div class="queue-row ${i < 2 ? "next" : ""}"><div class="player-id">#${i + 1}</div><div class="player-name">${esc(p.name)}</div><div class="badge icon-badge">${ICON_GAME}${esc(p.gameName)}</div><div class="badge icon-badge">${ICON_TROPHY}${esc(p.rank)}</div><div class="badge queue-status">${i < 2 ? "NEXT MATCH" : "WAITING"}</div></div>`,
        )
        .join("")
    : `<div class="empty-state">Queue is empty.</div>`;
}
function match(m) {
  $("#matchState").textContent = m.active ? "LIVE" : "WAITING";
  $("#arenaP1").textContent = m.active ? m.player1 : "Waiting";
  $("#arenaP2").textContent = m.active ? m.player2 : "Waiting";
}
function history(h) {
  $("#historyList").innerHTML = h.length
    ? h
        .map(
          (m) =>
            `<div class="history-row"><div class="player-id">#${m.matchId}</div><div>${esc(m.player1Name)}</div><div>${esc(m.player2Name)}</div><div class="player-name">${esc(m.winnerName)}</div><div>${esc(m.score)}</div><div>${esc(m.round)}</div><div>${esc(m.timestamp)}</div></div>`,
        )
        .join("")
    : `<div class="empty-state">No match history yet.</div>`;
}
function winners(w) {
  $("#winnersList").innerHTML = w.length
    ? w
        .map(
          (p, i) =>
            `<div class="winner-row"><div class="player-id">#${i + 1}</div><div>${esc(p.id)}</div><div class="player-name">${esc(p.name)}</div><div class="badge icon-badge">${ICON_GAME}${esc(p.gameName)}</div><div class="badge icon-badge">${ICON_TROPHY}${esc(p.rank)}</div></div>`,
        )
        .join("")
    : `<div class="empty-state">No winners yet.</div>`;
}
function page(p) {
  document
    .querySelectorAll(".page")
    .forEach((x) => x.classList.remove("active"));
  document
    .querySelectorAll(".nav-btn")
    .forEach((x) => x.classList.remove("active"));
  $(`#${p}Page`).classList.add("active");
  document.querySelector(`[data-page="${p}"]`).classList.add("active");
  $("#pageSubtitle").textContent =
    `${p[0].toUpperCase() + p.slice(1)} Control Center`;
}
function obj(f) {
  return Object.fromEntries(new FormData(f).entries());
}
function updateRankDropdown() {
  const game = $("#gameSelect").value;
  const rankSelect = $("#rankSelect");
  rankSelect.innerHTML = `<option value="">Select Rank</option>`;
  if (game && GAMES[game]) {
    GAMES[game].forEach((rank) => {
      const opt = document.createElement("option");
      opt.value = rank;
      opt.textContent = rank;
      rankSelect.appendChild(opt);
    });
  }
}
document
  .querySelectorAll(".nav-btn")
  .forEach((b) => (b.onclick = () => page(b.dataset.page)));
$("#gameSelect").onchange = updateRankDropdown;
$("#playerForm").onsubmit = async (e) => {
  e.preventDefault();
  const r = await post("/api/players/add", obj(e.target));
  if (!r.success) return toast(r.error, true);
  e.target.reset();
  updateRankDropdown();
  toast("Player added.");
  refresh();
};
$("#updatePlayerBtn").onclick = async () => {
  const r = await post("/api/players/update", obj($("#playerForm")));
  if (!r.success) return toast(r.error, true);
  toast("Player updated.");
  refresh();
};
$("#deletePlayerBtn").onclick = async () => {
  const id = new FormData($("#playerForm")).get("id");
  const r = await post("/api/players/delete", { id });
  if (!r.success) return toast(r.error, true);
  toast("Player deleted.");
  refresh();
};
$("#profileQueueBtn").onclick = async () => {
  if (!state.selectedPlayer) return toast("Select player first.", true);
  const r = await post("/api/queue/add", { id: state.selectedPlayer.id });
  if (!r.success) return toast(r.error, true);
  toast("Added to queue.");
  refresh();
};
$("#profileLoadBtn").onclick = () => {
  if (!state.selectedPlayer) return toast("Select player first.", true);
  const f = $("#playerForm");
  f.id.value = state.selectedPlayer.id;
  f.name.value = state.selectedPlayer.name;
  f.age.value = state.selectedPlayer.age;
  f.gameName.value = state.selectedPlayer.gameName;
  updateRankDropdown();
  f.rank.value = state.selectedPlayer.rank;
  toast("Loaded to form.");
};
$("#queueForm").onsubmit = async (e) => {
  e.preventDefault();
  const r = await post("/api/queue/add", obj(e.target));
  if (!r.success) return toast(r.error, true);
  e.target.reset();
  toast("Added to queue.");
  refresh();
};
$("#resetQueueBtn").onclick = async () => {
  await post("/api/queue/reset");
  toast("Queue reset.");
  refresh();
};
$("#startMatchBtn").onclick = async () => {
  const r = await post("/api/match/start");
  if (!r.success) return toast(r.error, true);
  page("matches");
  toast("Match started.");
  refresh();
};
$("#resultForm").onsubmit = async (e) => {
  e.preventDefault();
  const r = await post("/api/match/result", obj(e.target));
  if (!r.success) return toast(r.error, true);
  e.target.reset();
  toast("Result recorded.");
  refresh();
};
$("#clearHistoryBtn").onclick = async () => {
  await post("/api/history/clear");
  toast("History cleared.");
  refresh();
};
refresh();
setInterval(refresh, 4000);
