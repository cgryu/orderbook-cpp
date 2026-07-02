import matplotlib
matplotlib.use("Agg")  # no display needed, write straight to file
import matplotlib.pyplot as plt
import numpy as np

# --- Measured data: 5 matched, core-pinned runs each (Jul 1 2026) ---
# throughput in millions of ops/sec
mapbook  = [8.66, 8.83, 8.86, 8.80, 8.83]
flatbook = [9.95, 9.54, 9.54, 9.73, 9.13]   # 9.13 = one disturbed run (see README)
disturbed_flat = 9.13                         # mark it honestly

map_med  = float(np.median(mapbook))
flat_med = float(np.median(flatbook))
edge_pct = (flat_med / map_med - 1) * 100

MAP_C  = "#c1666b"   # muted red
FLAT_C = "#4d7c8a"   # muted teal
INK    = "#22252a"

fig, ax = plt.subplots(figsize=(7.2, 4.6), dpi=150)

groups = [("MapBook", mapbook, map_med, MAP_C, 0),
          ("FlatBook", flatbook, flat_med, FLAT_C, 1)]

rng = np.random.default_rng(7)  # seeded jitter for reproducibility
for name, data, med, color, x in groups:
    # shaded range band
    ax.add_patch(plt.Rectangle((x - 0.28, min(data)), 0.56, max(data) - min(data),
                               facecolor=color, alpha=0.10, edgecolor="none", zorder=1))
    # individual run points (jittered)
    for v in data:
        jx = x + rng.uniform(-0.10, 0.10)
        if name == "FlatBook" and v == disturbed_flat:
            ax.scatter(jx, v, s=70, facecolors="none", edgecolors=color,
                       linewidths=1.8, zorder=3)  # open marker = disturbed run
        else:
            ax.scatter(jx, v, s=70, color=color, alpha=0.9, zorder=3,
                       edgecolors="white", linewidths=0.8)
    # median line
    ax.plot([x - 0.28, x + 0.28], [med, med], color=color, lw=2.6, zorder=4)
    ax.annotate(f"median {med:.2f}M", (x + 0.30, med), va="center", ha="left",
                fontsize=9.5, color=color, fontweight="bold")

# non-overlap guide: MapBook max vs FlatBook min
ax.axhline(max(mapbook), color=INK, lw=0.8, ls=":", alpha=0.5, zorder=2)
ax.axhline(min(flatbook), color=INK, lw=0.8, ls=":", alpha=0.5, zorder=2)
ax.annotate("ranges do not overlap\n(FlatBook slowest > MapBook fastest)",
            xy=(0.5, (max(mapbook) + min(flatbook)) / 2),
            ha="center", va="center", fontsize=8.5, color=INK,
            bbox=dict(boxstyle="round,pad=0.35", fc="#f4f1ec", ec=INK, lw=0.6, alpha=0.95),
            zorder=5)

ax.set_xticks([0, 1])
ax.set_xticklabels(["MapBook\n(std::map)", "FlatBook\n(tick-indexed vector)"], fontsize=10)
ax.set_ylabel("Throughput  (million ops / sec)", fontsize=10.5, color=INK)
ax.set_title(f"Order-book throughput: FlatBook ~{edge_pct:.0f}% faster than MapBook\n"
             f"5 matched core-pinned runs each · identical seeded workload · –O3",
             fontsize=11, color=INK, pad=12)
ax.set_xlim(-0.5, 1.75)
ax.set_ylim(8.4, 10.15)
ax.spines[["top", "right"]].set_visible(False)
ax.spines[["left", "bottom"]].set_color("#9aa0a6")
ax.tick_params(colors="#5f6368")
ax.grid(axis="y", color="#e6e3de", lw=0.7, zorder=0)
ax.set_axisbelow(True)

# footnote on the open marker
ax.annotate("○ = one run with a machine disturbance (still clears MapBook)",
            xy=(0.5, -0.16), xycoords="axes fraction", ha="center",
            fontsize=8, color="#5f6368")

fig.tight_layout()
out = "benchmark_throughput.png"
fig.savefig(out, bbox_inches="tight", facecolor="white")
print(f"wrote {out}")
print(f"MapBook median {map_med:.2f}M (range {min(mapbook)}-{max(mapbook)})")
print(f"FlatBook median {flat_med:.2f}M (range {min(flatbook)}-{max(flatbook)})")
print(f"edge {edge_pct:.1f}%  | non-overlap: FlatBook min {min(flatbook)} > MapBook max {max(mapbook)} = {min(flatbook) > max(mapbook)}")