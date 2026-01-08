/**
 * ProjT Launcher - Team Information Fetcher
 * Fetches team information from GitHub organization for CI purposes
 */

// Teams to exclude from processing (bots, voters, etc.)
const excludeTeams = [
  /^voters.*$/,
  /^bots?$/,
]

/**
 * Main function to fetch team information
 */
module.exports = async ({ github, context, core, outFile }) => {
  const withRateLimit = require('./withRateLimit.js')
  const { writeFileSync } = require('node:fs')

  const org = context.repo.owner
  const result = {}

  await withRateLimit({ github, core }, async () => {
    /**
     * Convert array of users to object mapping login -> id
     */
    function makeUserSet(users) {
      users.sort((a, b) => (a.login > b.login ? 1 : -1))
      return users.reduce((acc, user) => {
        acc[user.login] = user.id
        return acc
      }, {})
    }

    /**
     * Process teams recursively
     */
    async function processTeams(teams) {
      for (const team of teams) {
        // Skip excluded teams
        if (excludeTeams.some((regex) => team.slug.match(regex))) {
          core.info(`Skipping excluded team: ${team.slug}`)
          continue
        }

        core.notice(`Processing team ${team.slug}`)
        
        try {
          // Get team members
          const members = makeUserSet(
            await github.paginate(github.rest.teams.listMembersInOrg, {
              org,
              team_slug: team.slug,
              role: 'member',
            }),
          )
          
          // Get team maintainers
          const maintainers = makeUserSet(
            await github.paginate(github.rest.teams.listMembersInOrg, {
              org,
              team_slug: team.slug,
              role: 'maintainer',
            }),
          )
          
          result[team.slug] = {
            description: team.description,
            id: team.id,
            maintainers,
            members,
            name: team.name,
          }
        } catch (e) {
          core.warning(`Failed to fetch team ${team.slug}: ${e.message}`)
        }

        // Process child teams
        try {
          const childTeams = await github.paginate(
            github.rest.teams.listChildInOrg,
            {
              org,
              team_slug: team.slug,
            },
          )
          await processTeams(childTeams)
        } catch (e) {
          // Child teams might not exist or be accessible
          core.info(`No child teams for ${team.slug}`)
        }
      }
    }

    // Get all teams with access to the repository
    try {
      const teams = await github.paginate(github.rest.repos.listTeams, {
        ...context.repo,
      })
      
      core.info(`Found ${teams.length} teams with repository access`)
      await processTeams(teams)
    } catch (e) {
      core.warning(`Could not fetch repository teams: ${e.message}`)
      
      // Fallback: create minimal team structure
      result['projt-maintainers'] = {
        description: 'ProjT Launcher Maintainers',
        id: 0,
        maintainers: {},
        members: {},
        name: 'ProjT Maintainers',
      }
    }
  })

  // Sort teams alphabetically
  const sorted = Object.keys(result)
    .sort()
    .reduce((acc, key) => {
      acc[key] = result[key]
      return acc
    }, {})

  const json = `${JSON.stringify(sorted, null, 2)}\n`

  if (outFile) {
    writeFileSync(outFile, json)
    core.info(`Team information written to ${outFile}`)
  } else {
    console.log(json)
  }

  return sorted
}
