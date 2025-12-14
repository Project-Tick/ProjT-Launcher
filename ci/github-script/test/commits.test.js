#!/usr/bin/env node
'use strict'

process.env.COMMIT_TYPES = 'customtype'

const assert = require('node:assert/strict')
const commits = require('../commits.js')

const { validateCommitMessage, normalizeCommitType } = commits

const validMessages = [
  'feat(ui): add redesigned settings panel',
  'refactor: drop deprecated launcher flag support',
  'chore(ci): refresh workflows configuration',
  '11.feat: support legacy numbered commit type format',
  '23.deps(deps): bump dependency pins',
  'release: publish stable build artifacts',
  'customtype: allow env commit type overrides',
]

for (const message of validMessages) {
  const result = validateCommitMessage(message)
  assert.equal(
    result.valid,
    true,
    `Expected commit "${message}" to be valid, got: ${result.message}`
  )
}

const invalidType = validateCommitMessage('unknown(scope): add feature that is real enough')
assert.equal(invalidType.valid, false, 'Expected invalid type to be rejected')
assert.match(invalidType.message, /Unknown commit type/i)

const shortDescription = validateCommitMessage('feat: short')
assert.equal(shortDescription.valid, false, 'Expected short description to fail validation')
assert.match(shortDescription.message, /too short/i)

assert.equal(normalizeCommitType('11.feat'), 'feat')
assert.equal(normalizeCommitType('23.deps'), 'deps')
assert.equal(normalizeCommitType('chore'), 'chore')

console.log('commits.js tests passed')
