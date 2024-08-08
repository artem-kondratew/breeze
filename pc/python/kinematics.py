def constrain(vel):
    vel = +100 if vel > +100 else vel
    vel = -100 if vel < -100 else vel

    return vel


def calcVelocities(vx, wz):
    v0 = vx - wz
    v1 = vx + wz

    return int(constrain(v0)), int(constrain(v1))


if __name__ == '__main__':
    while True:
        vx, wz = list(map(int, input().split()))
        print(calcVelocities(vx, wz))