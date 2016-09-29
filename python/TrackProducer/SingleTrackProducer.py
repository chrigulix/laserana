import numpy as np
import pprint

def Track(Plane, StartPosition, EndPosition, RawHitDef):
    """ Produces a track along the line in wire / time tick space space with the specificed
    shape.

    Params:
        - StartPosition:    Start position in the form (Wire, TimeTick)
        - EndPosition:      End position in the same format as start position
        - RawHitDef:        Base definiton of the shape of the single hits in the form of:
                            [width, amplitude, offset]
    Output:
        - np.array of hits of the form [plane, wire, tick, width, amplitude, offset, multipicity] ordered along the
          wire number.

    """

    StartWire = StartPosition[0]
    EndWire = EndPosition[0]

    StartTick = StartPosition[1]
    EndTick = EndPosition[1]

    wire_range = abs(EndWire - StartWire)

    gradient = (EndTick - StartTick) / float(wire_range)
    hits = -1 * np.ones((abs(wire_range + 1), 7), dtype=int)

    # handle inverted case
    if EndWire < StartWire:
        EndWire, StartWire = StartWire, EndWire

    for wire in range(StartWire, EndWire + 1):
        tick = int(np.around(wire * gradient + StartTick))
        hits[wire - StartWire, :] = [Plane, wire, tick, RawHitDef[0], RawHitDef[1], RawHitDef[2], 1]

    return hits


def Event(Plane, TrackDefinitions, HitDefinitions):
    """ Produces a list of all hits representing all tracks based on the TrackDefinitions within a single event.

    Params:
        - TrackDefinistions:    List of tuples containing the Start / End Position of each track in the following form:
                                [(Start1, End1), (Start2, End2), ... (StartN,EndN)] with
                                (StartX,EndX) = ((WireStart,TimStart), (WireEnd,TimeEnd))
        - HitDefinitions        List of width, amplitude, offset for each track
    Output:
        - ordered np.array containg all raw hits based on wire and time tick
    """


    for plane, track, hit_def in zip(Plane, TrackDefinitions,HitDefinitions):
        track = Track(plane, track[0], track[1], hit_def)
        print track


def Run():
    """ Produces a list of all hits representing the tracks based on the TrackDefinitions within the whole run.
        Currently only a single track in an event is supported.

    Params:
        - TrackDefinistions:    List of tuples containing the Start / End Position of each track in the following form:
                                [(Start1, End1), (Start2, End2), ... (StartN,EndN)] with
                                (StartX,EndX) = ((WireStart,TimStart), (WireEnd,TimeEnd))
    Output:
        - ordered np.array containg all raw hits based on wire and time tick
    """
    pass


def ProduceSingleTrack(filename, Plane, TrackDefinitions, HitDefinitions):
    """ Assume each track goes with a new event"""

    with open(filename + ".txt", "w+") as file:
        file.write("# plane, wire, tick, width, amplitude, offset, multipicity\n")
        for event, plane, track, hit_def in zip( range(len(Plane)), Plane, TrackDefinitions,HitDefinitions):
            track = Track(plane, track[0], track[1], hit_def)

            file.write("# event " + str(event) + "\n")
            for hit in track:
                string = ", ".join([str(num) for num in hit.tolist()])
                file.write(string + "\n")

        file.write("# finished\n")


planes= [0, 0, 0, 0]
edges = [[(0, 3500), (50, 3600)],
         [(0, 3500), (50, 3600)],
         [(0, 3500), (50, 3600)]
         ]

hit_defs = [[5, 5, 5],
            [10, 10, 10],
            [20, 20, 20]]

ProduceSingleTrack("test", planes, edges, hit_defs)