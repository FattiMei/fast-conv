import re
import sys
import json
import numpy as np
import matplotlib.pyplot as plt


def get_time_unit(unit):
    return {
        's':  1e0,
        'ms': 1e-3,
        'us': 1e-6,
        'ns': 1e-9
    }[unit]


def process_benchmark(benchmark):
    name, n = re.findall(
        r'BM_automata_step<(.*)>/(.*)',
        benchmark['name']
    )[0]

    nelements = int(n)
    time = benchmark['real_time'] * get_time_unit(benchmark['time_unit'])
    throughput = (nelements / 2**30) / time

    return name, nelements, throughput


def parse(json_path):
    result = {}

    with open(json_path, 'r') as file:
        report = json.load(file)

    for bench in report['benchmarks']:
        name, nelements, throughput = process_benchmark(bench)

        try:
            result[name]['elements'].append(nelements)
            result[name]['throughput'].append(throughput)

        except KeyError:
            result[name] = {}
            result[name]['elements'] = [nelements]
            result[name]['throughput'] = [throughput]

    for name, data in result.items():
        result[name]['elements'] = np.array(data['elements'])
        result[name]['throughput'] = np.array(data['throughput'])

    return result


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python plot.py <json report>")
        sys.exit(1)

    results = parse(sys.argv[1])


    # ---------------------------- scalability plot ---------------------------
    plt.title("automaton scalability")
    plt.xlabel("size [elements]")
    plt.ylabel("throughput [Gb/s]")

    for name, data in results.items():
        plt.semilogx(data['elements'], data['throughput'], label=name)

    plt.legend(framealpha=1)

    try:
        plt.show()

    except:
        print("Can't plot on screen, saving to file")
        plt.savefig('automaton_scalability.svg', format='svg')
        plt.savefig('automaton_scalability.png', format='png')


    # -------------------------------speedup plot ------------------------------
    reference = results['compute_new_line_reference']['throughput']

    plt.title("implementation speedup")
    plt.xlabel("size [elements]")
    plt.ylabel("speedup")

    for name, data in results.items():
        plt.semilogx(data['elements'], data['throughput'] / reference, label=name)

    plt.legend(framealpha=1)

    try:
        plt.show()

    except:
        plt.savefig('automaton_speedup.svg', format='svg')
        plt.savefig('automaton_speedup.png', format='png')
