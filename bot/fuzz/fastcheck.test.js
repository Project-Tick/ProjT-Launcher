import assert from "node:assert/strict";
import { test } from "node:test";

import fc from "fast-check";

import { parsePositiveInt, safeJsonParse } from "../index.js";

test("safeJsonParse never throws", () => {
  fc.assert(
    fc.property(fc.string(), (value) => {
      const result = safeJsonParse(value);
      assert.ok(result === null || typeof result === "object");
    }),
    { numRuns: 1000 }
  );
});

test("parsePositiveInt returns null or positive integer", () => {
  fc.assert(
    fc.property(fc.anything(), (value) => {
      const result = parsePositiveInt(value);
      if (result === null) return;
      assert.ok(Number.isInteger(result));
      assert.ok(result > 0);
    }),
    { numRuns: 1000 }
  );
});
