from lar_utils import *


def calc_residual(track, laser, axes=None):
    # these are uboone coordinates!
    x, y, z = track[1], track[2], track[3]

    laser_entry = np.rec.array([laser[1], laser[2], laser[3]],
                               dtype = [('x', 'f'), ('y', 'f'), ('z', 'f')])
    laser_exit = np.rec.array([laser[4],laser[5],laser[6]],
                              dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    laser_dir = np.rec.array([laser[7],laser[8],laser[9]],
                             dtype=[('x', 'f'), ('y', 'f'), ('z', 'f')])

    corr_entry = 1
    corr_exit = 20

    laser_entry.x = laser_entry.x + corr_entry
    laser_exit.x = laser_exit.x + corr_exit


    laser_entry_zx = [laser_entry.z, laser_entry.x]
    laser_exit_zx = [laser_exit.z, laser_exit.x]

    laser_entry_zy = [laser_entry.z, laser_entry.y]
    laser_exit_zy = [laser_exit.z, laser_exit.y]

    print laser_dir.y
    # select more or less straight
    if not (laser_dir.x > -0.04 and not(0.08 > laser_dir.y > 0.042)):
        return

    m_laser, b_laser = calc_line(laser_entry_zx, laser_exit_zx)
    m, b = calc_line_slope([z, x], -1/m_laser)
    z_inters, x_inters = calc_intersect(m_laser, b_laser, m, b)
    r_zx = calc_distance([z_inters, x_inters], [z, x])

    m_laser, b_laser = calc_line(laser_entry_zy, laser_exit_zy)
    m, b = calc_line_slope([z, y], -1/m_laser)


    z_inters, y_inters = calc_intersect(m_laser, b_laser, m, b)

    r_zy = calc_distance([z_inters, y_inters], [z, y])

    axes[0].plot(z, r_zx, "*")
    axes[1].plot(z, r_zy, "*")


tracks_filename = "data/laser-tracks-7205.npy"
laser_filename = "data/laser-data-7205-calib.npy"

track_data = np.load(tracks_filename)
laser_data = np.load(laser_filename)

fig = plt.figure(figsize=(15, 6), dpi=120)

ax_zx = fig.add_subplot(211)
ax_zy = fig.add_subplot(212)
axes = [ax_zx, ax_zy]

for track, laser in zip(track_data, laser_data):
    calc_residual(track, laser, axes=axes)

reco_chain = "roi,laserhit,kalman"

ax_zx.set_title("Residuals for fixed position (reco: {})".format(reco_chain))

ax_zx.set_xlim([0, 1056])
ax_zx.set_ylim([0, 25])
ax_zx.set_ylabel("x-residual [cm]")

ax_zy.set_xlim([0, 1056])
ax_zy.set_ylim([0, 70])
ax_zy.set_xlabel("z [cm]")
ax_zy.set_ylabel("y-residual [cm]")

plt.show()

figure_filename = "output/residuals-{}.png".format(reco_chain.replace(',','-'))
fig.savefig(figure_filename)