import numpy as np

def Track(Plane, StartPosition, EndPosition, RawHitDef):
    """ Produces a track along the line in wire / time tick space space with the specificed
    shape.

    Params:
        - StartPosition:    Start position in the form (Wire, TimeTick)
        - EndPosition:      End position in the same format as start position
        - RawHitDef:        Base definiton of the shape of the single hits in the form of:
                            [width, amplitude, offset]
    Output:
        - np.array of hits of the form [plane, wire, tick, width, amplitude, offset, multipicity]

    """

    StartWire = StartPosition[0]
    EndWire = EndPosition[0]

    StartTick = StartPosition[1]
    EndTick = EndPosition[1]

    wire_range = abs(EndWire - StartWire) + 1

    print wire_range

    gradient = (EndTick - StartTick) / float(abs(EndWire - StartWire))

    hits = -1 * np.ones((abs(wire_range),7), dtype=int)

    # handle inverted case
    if (EndWire < StartWire):
        EndWire, StartWire = StartWire, EndWire

    for wire in range(StartWire, EndWire+1):
        tick = int(np.around(wire * gradient + StartTick))
        print wire - StartWire
        hits[wire - StartWire, :] = [Plane, wire, tick, RawHitDef[0], RawHitDef[1], RawHitDef[2], 1]

    return hits

def Event(TrackDefinitions):
    """" Produces a list of all hits representing all tracks based on the TrackDefinitions within a single event.

    Params:
        - TrackDefinistions:    List of tuples containing the Start / End Position of each track in the following form:
                                [(Start1, End1), (Start2, End2), ... (StartN,EndN)] with
                                (StartX,EndX) = ((WireStart,TimStart), (WireEnd,TimeEnd))
    Output:
        - ordered np.array containg all raw hits based on wire and time tick
    """
    pass

def Run(Events):
    pass


Track(0, (5,5), (0,5), [5,10,20])