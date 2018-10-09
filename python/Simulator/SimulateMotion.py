from mpl_toolkits.mplot3d import Axes3D
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

from CommonIncludes import PC_STATUS, AXES

class SimulateMotion:

    def __init__(self, map, status_locations, json):
        self._fig = None
        self._ax = None
        self._phase_map = map
        self._status_locations = status_locations
        self._json = json
        self._rpc = [json['targetGenerator']['boxStart'], json['targetGenerator']['boxEnd']]
        self._rpc_color = self._colorsToMap('RPC')
        self._rpc_opacity = 0.2



    def modelData(self):
        for session, phases in self._phase_map.items():
            self._fig = plt.figure()
            self._ax = self._fig.gca(projection='3d')
            self._plotRPCBin()

            for phase, df in phases.items():
                self._ax.plot3D(pd.to_numeric(df['X']), pd.to_numeric(df['Y']), pd.to_numeric(df['Z']),
                                self._colorsToMap(phase), label="({})_{}".format(session, phase))
            self._ax.set_title("Session {}".format(session))
            self._ax.set_xlabel('X')
            self._ax.set_ylabel('Y')
            self._ax.set_zlabel('Z')
            self._ax.legend()
            plt.show()

    def _colorsToMap(self, phase):
        color_mapping = {
            'ZERO'  : 'blue',
            'PICK'  : 'magenta',
            'DROP'  : 'orange',
            'RPC'   : 'black'
        }
        return color_mapping[phase]

    def _plotRPCBin(self):
        """
        IDE: when running in IDE comment out AXES.<label> lines that don't
            call the value of the enumeration.
        TERMINAL: when running in terminal comment out '.value' lines.
        """
        self._rect_prism(
            np.array([self._rpc[0][AXES.X.value], self._rpc[1][AXES.X.value]]),
            np.array([self._rpc[0][AXES.Y.value], self._rpc[1][AXES.Y.value]]),
            np.array([self._rpc[0][AXES.Z.value], self._rpc[1][AXES.Z.value]])
            # np.array([self._rpc[0][AXES.X], self._rpc[1][AXES.X]]),
            # np.array([self._rpc[0][AXES.Y], self._rpc[1][AXES.Y]]),
            # np.array([self._rpc[0][AXES.Z], self._rpc[1][AXES.Z]])
        )

    def _rect_prism(self, x_range, y_range, z_range):
        self._x_y_edge(x_range, y_range, z_range)
        self._y_z_edge(x_range, y_range, z_range)
        self._x_z_edge(x_range, y_range, z_range)

    def _x_y_edge(self, x_range, y_range, z_range):
        xx, yy = np.meshgrid(x_range, y_range)
        zz = self._format_surface(z_range)

        for i in range(2):
            self._ax.plot_wireframe(xx, yy, zz[i], color=self._rpc_color)
            self._ax.plot_surface(xx, yy, zz[i], color=self._rpc_color, alpha=self._rpc_opacity)

    def _y_z_edge(self, x_range, y_range, z_range):
        yy, zz = np.meshgrid(y_range, z_range)
        xx = self._format_surface(x_range)

        for i in range(2):
            self._ax.plot_wireframe(xx[i], yy, zz, color=self._rpc_color)
            self._ax.plot_surface(xx[i], yy, zz, color=self._rpc_color, alpha=self._rpc_opacity)

    def _x_z_edge(self, x_range, y_range, z_range):
        xx, zz = np.meshgrid(x_range, z_range)

        yy = self._format_surface(y_range)
        for i in range(2):
            self._ax.plot_wireframe(xx, yy[i], zz, color=self._rpc_color)
            self._ax.plot_surface(xx, yy[i], zz, color=self._rpc_color, alpha=self._rpc_opacity)

    def _format_surface(self, range):
        return [
            np.array(
                [
                    np.repeat(range[0], 2),
                    np.repeat(range[0], 2)
                ],
            ),
            np.array(
                [
                    np.repeat(range[1], 2),
                    np.repeat(range[1], 2)
                ]
            )
        ]

    def _add_arrow(self, df):
        """
        Arrows should be placed in the direction of travel separated from each other
            by a distance proportial to the length of the phase.
            (ie.: each 1/4)
        :param df: dataframe of specific phase
        :return:
        """

        # NOTE: dataframes must be the same shape so it doesn't matter which axis we check the
        # size of.
        df = df.reset_index(drop=True)
        df_len = df['X'].size
        normal = 100

        scaling_factor = round(df_len / normal - 1) if round(df_len / normal - 1) > 1 else 2
        increments = round(df_len / scaling_factor)
        indexes = list(np.arange(increments, df_len - increments, increments))

        for i in indexes:
            xyz = list(map(int, df.loc[[i]].values.flatten()))
            uvw = list(map(int, df.loc[[i - 50]].values.flatten()))
            self._ax.quiver(xyz[AXES.X.value], xyz[AXES.Y.value], xyz[AXES.Z.value],
                            25, 25, 25,
                            pivot='middle')

    def _add_endpoint_status(self, df):
        used_coord = []
        for status, coord_set in self._status_locations.items():
            for coord in coord_set:
                if coord[1] not in used_coord:
                    self._ax.text(int(coord[1][0]), int(coord[1][1]), int(coord[1][2]), status.name)
                    used_coord.append(coord[1])
