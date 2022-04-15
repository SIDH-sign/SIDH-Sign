# 2-dimensional pohlig hellman: optimal strategy (order-3ᵇ point decomposition)
import click


def dpa(n: int, mul: float, sqr: float, add: float):
    """
    dynamic programming algorithm for optimal 2-dimensional Pohlig Hellman strategy search
    :param n: exponent of 3
    :param mul: GF(p²)-multiplication cost
    :param sqr: GF(p²)-square cost
    :param add: GF(p²)-addition cost
    :return:
    """
    # On Twisted Edwards curves we have
    # p determines point tripling in projective coordinates
    p = (10.0 * mul) + (3.0 * sqr) + (9.0 * add)
    # q determines average cost for computing cᵢ and dᵢ both in {0,1,2} by assuming uniform distribution
    # We have 9 options: [(0 ,0), (1, 1), (2, 2), (0, 1), (0, 2), (1, 2), (1, 0), (2, 0), (2, 1)]. So, we have q equals
    # cost(0 ,0) + cost(1, 1) + cost(2, 2) + cost(0, 1) + cost(0, 2) + cost(1, 2) + cost(1, 0) + cost(2, 0) + cost(2, 1)
    q = ((164.0 * mul) + (32.0 * sqr) + (119.0 * add)) / 9.0

    strategy = {1: []}
    cost = {1: 0.00}
    for i in range(2, n + 1):
        b, c = min(((b, cost[i - b] + cost[b] + b * p + (i - b) * q) for b in range(1, i)), key=lambda t: t[1])
        strategy[i] = [b] + strategy[i - b] + strategy[b]
        cost[i] = c

    return strategy[n], cost[n]


@click.command()
@click.option('--b', default=137, help='Integer b determining order-3ᵇ points')
@click.option('--mul', default=1.00, help='GF(p²)-multiplication cost')
@click.option('--sqr', default=0.70, help='GF(p²)-square cost')
@click.option('--add', default=0.01, help='GF(p²)-addition cost')
def main(b: int, mul: float, sqr: float, add: float):
    strategy, cost = dpa(b, mul, sqr, add)
    assert (len(strategy) == (b - 1))
    click.echo(f'static const unsigned int STRATEGY_POHLIG_HELLMAN[NUMBER_OF_ISOGENIES_BOB - 1] = {strategy}')
    click.echo(f'cost:\t{cost}')


if __name__ == '__main__':
    main()
