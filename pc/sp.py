"""
signal processing. Determine what is happening to the ball from
a stream of accelerations. The code here is generator based, i.e. event
based.
"""

def sum_squares_gen(v3_gen, verbose=False):
    for x, y, z in v3_gen:
        sq = x**2 + y**2 + z**2
        if verbose:
            print sq
        yield sq

def throws(accels, initial = 0, throw_max_accel=0.1, land_min_accel=0.5, verbose=False):
    """ generator, yields (<throw num>, 1) when thrown, and (<throw num>, 0)
    when landed. Throw is detected
    """
    i = initial
    sq_gen = sum_squares_gen(accels, verbose=verbose)
    while True:
        for a in sq_gen:
            if a < throw_max_accel:
                i += 1
                yield i, 1
                break
        for a in sq_gen:
            if a > land_min_accel:
                yield i, 0
                break

