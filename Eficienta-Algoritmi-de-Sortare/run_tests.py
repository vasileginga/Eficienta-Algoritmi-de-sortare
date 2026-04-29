from __future__ import annotations

import subprocess
import sys
import time
from pathlib import Path


CONFIG_PATH = Path("config.txt")
EXECUTABLE_PATH = Path("Algorithm-Efficiency.exe")
DATASET_TYPES = {
    "random": 1,
    "sorted": 2,
    "reverse": 3,
    "almost_sorted": 4,
    "few_unique": 5,
}
RUN_DELAY_SECONDS = 1
RUN_TIMEOUT_SECONDS = 7200
SKIP_SIZE_THRESHOLD = 1000000
SKIP_DATASET_TYPES = {"random", "reverse"}


def load_config(config_path: Path) -> tuple[int, list[int]]:
    if not config_path.is_file():
        raise FileNotFoundError(f"Fisierul de configurare lipseste: {config_path}")

    runs: int | None = None
    sizes: list[int] = []

    with config_path.open("r", encoding="utf-8") as config_file:
        for raw_line in config_file:
            line = raw_line.strip()
            if not line or "=" not in line:
                continue

            key, value = line.split("=", 1)
            key = key.strip().lower()
            value = value.strip()

            if key == "runs":
                try:
                    parsed_runs = int(value)
                except ValueError:
                    continue

                if parsed_runs > 0:
                    runs = parsed_runs

            elif key == "sizes":
                parsed_sizes: list[int] = []
                for token in value.split(","):
                    item = token.strip()
                    if not item:
                        continue
                    try:
                        parsed_size = int(item)
                    except ValueError:
                        continue
                    if parsed_size > 0:
                        parsed_sizes.append(parsed_size)

                if parsed_sizes:
                    sizes = parsed_sizes

    if runs is None:
        raise ValueError("Valoarea 'runs' lipseste sau este invalida in config.txt")
    if not sizes:
        raise ValueError("Lista 'sizes' lipseste sau este invalida in config.txt")

    return runs, sizes


def build_benchmark_input(dataset_type_number: int, size: int) -> str:
    return f"2\n{dataset_type_number}\n{size}\n\n\n\n\n"


def build_averages_input() -> str:
    return "3\n\n"


def should_skip_run(dataset_name: str, size: int) -> bool:
    return size > SKIP_SIZE_THRESHOLD and dataset_name in SKIP_DATASET_TYPES


def run_executable(executable_path: Path, provided_input: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [str(executable_path)],
        input=provided_input,
        text=True,
        check=True,
        timeout=RUN_TIMEOUT_SECONDS,
    )


def print_error_details(prefix: str, error_text: str) -> None:
    cleaned_text = error_text.strip()
    if cleaned_text:
        print(f"{prefix}: {cleaned_text}")
    else:
        print(prefix)


def main() -> int:
    script_dir = Path(__file__).resolve().parent
    config_path = script_dir / CONFIG_PATH
    executable_path = script_dir / EXECUTABLE_PATH

    try:
        runs, sizes = load_config(config_path)
    except (FileNotFoundError, ValueError) as error:
        print(f"Eroare configurare: {error}")
        return 1

    if not executable_path.is_file():
        print(f"Eroare: executabilul nu exista: {executable_path}")
        return 1

    total_runs = len(DATASET_TYPES) * len(sizes) * runs
    successful_runs = 0
    failed_runs = 0
    executed_runs = 0

    print(f"Executabil: {executable_path}")
    print(f"Rulari per combinatie: {runs}")
    print(f"Dimensiuni: {', '.join(str(size) for size in sizes)}")
    print(f"Total rulari planificate: {total_runs}")

    for dataset_name, dataset_type_number in DATASET_TYPES.items():
        for size in sizes:
            for run_index in range(1, runs + 1):
                executed_runs += 1
                print(
                    f"[{executed_runs}/{total_runs}] "
                    f"Tip={dataset_name} | Dimensiune={size} | Rulare={run_index}/{runs}"
                )

                if should_skip_run(dataset_name, size):
                    print(
                        f"Rulare omisa pentru Tip={dataset_name} si Dimensiune={size} "
                        f"din cauza pragului de siguranta ({SKIP_SIZE_THRESHOLD})."
                    )
                    continue

                try:
                    run_executable(
                        executable_path,
                        build_benchmark_input(dataset_type_number, size),
                    )
                    successful_runs += 1
                except subprocess.CalledProcessError as error:
                    failed_runs += 1
                    print_error_details(
                        f"Eroare la rularea executabilului pentru {dataset_name}, n={size}, rularea {run_index}",
                        (error.stderr or "") if isinstance(error.stderr, str) else "",
                    )
                except subprocess.TimeoutExpired:
                    failed_runs += 1
                    print(
                        f"Timeout la rularea executabilului pentru {dataset_name}, "
                        f"n={size}, rularea {run_index} dupa {RUN_TIMEOUT_SECONDS} secunde."
                    )
                except OSError as error:
                    failed_runs += 1
                    print(f"Eroare de sistem la pornirea executabilului: {error}")

                time.sleep(RUN_DELAY_SECONDS)

    averages_created = False
    try:
        print("Se calculeaza mediile din toate fisierele CSV...")
        run_executable(executable_path, build_averages_input())
        averages_created = True
    except subprocess.CalledProcessError as error:
        print_error_details(
            "Eroare la calcularea mediilor",
            (error.stderr or "") if isinstance(error.stderr, str) else "",
        )
    except subprocess.TimeoutExpired:
        print(f"Timeout la calcularea mediilor dupa {RUN_TIMEOUT_SECONDS} secunde.")
    except OSError as error:
        print(f"Eroare de sistem la calcularea mediilor: {error}")

    print("Rezumat final:")
    print(f"Rulari reusite: {successful_runs}")
    print(f"Rulari esuate: {failed_runs}")
    print(f"Total rulari executate: {executed_runs}")
    print(f"Medii generate: {'da' if averages_created else 'nu'}")

    return 0 if failed_runs == 0 and averages_created else 1


if __name__ == "__main__":
    sys.exit(main())