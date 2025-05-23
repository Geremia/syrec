# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

import pytest

from mqt import syrec

test_dir = Path(__file__).resolve().parent.parent
configs_dir = test_dir / "configs"
circuit_dir = test_dir / "circuits"


@pytest.fixture
def data_line_aware_synthesis():
    with (configs_dir / "circuits_line_aware_synthesis.json").open() as f:
        return json.load(f)


@pytest.fixture
def data_cost_aware_synthesis():
    with (configs_dir / "circuits_cost_aware_synthesis.json").open() as f:
        return json.load(f)


@pytest.fixture
def data_line_aware_simulation():
    with (configs_dir / "circuits_line_aware_simulation.json").open() as f:
        return json.load(f)


@pytest.fixture
def data_cost_aware_simulation():
    with (configs_dir / "circuits_cost_aware_simulation.json").open() as f:
        return json.load(f)


def test_parser(data_line_aware_synthesis: dict[str, Any]) -> None:
    for file_name in data_line_aware_synthesis:
        prog = syrec.program()
        error = prog.read(str(circuit_dir / (file_name + ".src")))

        assert not error


def test_synthesis_no_lines(data_line_aware_synthesis: dict[str, Any]) -> None:
    for file_name in data_line_aware_synthesis:
        circ = syrec.circuit()
        prog = syrec.program()
        error = prog.read(str(circuit_dir / (file_name + ".src")))

        assert not error
        assert syrec.line_aware_synthesis(circ, prog)
        assert data_line_aware_synthesis[file_name]["num_gates"] == circ.num_gates
        assert data_line_aware_synthesis[file_name]["lines"] == circ.lines
        assert data_line_aware_synthesis[file_name]["quantum_costs"] == circ.quantum_cost()
        assert data_line_aware_synthesis[file_name]["transistor_costs"] == circ.transistor_cost()


def test_synthesis_add_lines(data_cost_aware_synthesis: dict[str, Any]) -> None:
    for file_name in data_cost_aware_synthesis:
        circ = syrec.circuit()
        prog = syrec.program()
        error = prog.read(str(circuit_dir / (file_name + ".src")))

        assert not error
        assert syrec.cost_aware_synthesis(circ, prog)
        assert data_cost_aware_synthesis[file_name]["num_gates"] == circ.num_gates
        assert data_cost_aware_synthesis[file_name]["lines"] == circ.lines
        assert data_cost_aware_synthesis[file_name]["quantum_costs"] == circ.quantum_cost()
        assert data_cost_aware_synthesis[file_name]["transistor_costs"] == circ.transistor_cost()


def test_simulation_no_lines(data_line_aware_simulation: dict[str, Any]) -> None:
    for file_name in data_line_aware_simulation:
        circ = syrec.circuit()
        prog = syrec.program()
        error = prog.read(str(circuit_dir / (file_name + ".src")))

        assert not error
        assert syrec.line_aware_synthesis(circ, prog)

        my_inp_bitset = syrec.bitset(circ.lines)
        my_out_bitset = syrec.bitset(circ.lines)
        my_out_bitset2 = syrec.bitset(circ.lines)
        set_list = data_line_aware_simulation[file_name]["set_lines"]

        for set_index in set_list:
            my_inp_bitset.set(set_index, True)

        syrec.simple_simulation(my_out_bitset, circ, my_inp_bitset)
        assert data_line_aware_simulation[file_name]["sim_out"] == str(my_out_bitset)

        # Check reversed simulation
        syrec.simple_simulation(my_out_bitset2, circ, my_out_bitset, reverse=True)
        assert str(my_out_bitset2) == str(my_inp_bitset)


def test_simulation_add_lines(data_cost_aware_simulation: dict[str, Any]) -> None:
    for file_name in data_cost_aware_simulation:
        circ = syrec.circuit()
        prog = syrec.program()
        error = prog.read(str(circuit_dir / (file_name + ".src")))

        assert not error
        assert syrec.cost_aware_synthesis(circ, prog)

        my_inp_bitset = syrec.bitset(circ.lines)
        my_out_bitset = syrec.bitset(circ.lines)
        my_out_bitset2 = syrec.bitset(circ.lines)
        set_list = data_cost_aware_simulation[file_name]["set_lines"]

        for set_index in set_list:
            my_inp_bitset.set(set_index, True)

        syrec.simple_simulation(my_out_bitset, circ, my_inp_bitset)
        assert data_cost_aware_simulation[file_name]["sim_out"] == str(my_out_bitset)

        # Check reversed simulation
        syrec.simple_simulation(my_out_bitset2, circ, my_out_bitset, reverse=True)
        assert str(my_out_bitset2) == str(my_inp_bitset)


def test_no_lines_to_qasm(data_line_aware_synthesis: dict[str, Any]) -> None:
    for file_name in data_line_aware_synthesis:
        circ = syrec.circuit()
        prog = syrec.program()
        prog.read(str(circuit_dir / (file_name + ".src")))
        assert circ.to_qasm_file(str(circuit_dir / (file_name + ".qasm")))
