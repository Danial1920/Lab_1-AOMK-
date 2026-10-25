#!/usr/bin/env python3
"""Negative tests of reference-validation code, not numerical component tests."""
import copy
import json
import math
import unittest
from reference_cases import CASES, parse_component, compare

class ValidationTests(unittest.TestCase):
    def setUp(self):
        # Synthetic parser fixtures only; numerical reference commands always
        # execute the actual component and independent library solver.
        self.rows = [{'case': name, 'method': method, 'coefficients': [0.0]*(case[2]+1)}
                     for name, case in CASES.items() for method in (0,1)]
    def encode(self, rows):
        return '\n'.join('REFERENCE '+json.dumps(row) for row in rows)
    def test_valid_fixture(self):
        self.assertEqual(len(parse_component(self.encode(self.rows))), len(CASES)*2)
    def test_nonfinite(self):
        for value in (math.nan, math.inf, -math.inf):
            rows = copy.deepcopy(self.rows); rows[0]['coefficients'][0] = value
            with self.assertRaises(ValueError): parse_component(self.encode(rows))
    def test_truncated_or_extra_coefficients(self):
        for values in ([], [0.0], [0.0]*50):
            rows = copy.deepcopy(self.rows); rows[0]['coefficients'] = values
            with self.assertRaises(ValueError): parse_component(self.encode(rows))
    def test_missing_case_or_algorithm(self):
        with self.assertRaises(ValueError): parse_component(self.encode(self.rows[:-1]))
    def test_duplicate(self):
        with self.assertRaises(ValueError): parse_component(self.encode(self.rows+[self.rows[0]]))
    def test_unknown_case_or_method(self):
        for key, value in [('case','invented'),('method',3),('method',True)]:
            rows = copy.deepcopy(self.rows); rows[0][key] = value
            with self.assertRaises(ValueError): parse_component(self.encode(rows))
    def test_reference_shape_and_nonfinite(self):
        component = parse_component(self.encode(self.rows))
        reference = {name: [0.0]*(case[2]+1) for name,case in CASES.items()}
        for invalid in ([math.nan]*3, [], [1.0]):
            bad = copy.deepcopy(reference); bad['quadratic'] = invalid
            with self.assertRaises(ValueError): compare(component,bad,'fixture')
    def test_real_mismatch_fails(self):
        component = parse_component(self.encode(self.rows))
        reference = {name: [0.0]*(case[2]+1) for name,case in CASES.items()}
        reference['quadratic'][0] = 1.0
        with self.assertRaises(AssertionError): compare(component,reference,'fixture')

if __name__ == '__main__': unittest.main(verbosity=2)
