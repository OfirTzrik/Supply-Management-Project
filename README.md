# Simple 2-Person Git Workflow (No Branch Protection)

This README is a practical command reference for a small college project using a private repo.
It assumes the default branch is `main` (replace with `master` if your repo uses that).

---

## Quick rules (so you don’t step on each other)

- **Do not work directly on `main`.** Create a branch for every task/feature/fix.
- **Pull `main` before starting work** and before merging.
- **Push your branch at the end of the day** (or sooner) so nothing is lost.
- Prefer **PRs** (even without rulesets) to review and reduce accidental overwrites.

---

## 0) One-time setup (per machine)

### Install & configure identity (only once)
```bash
git config --global user.name "Your Name"
git config --global user.email "you@example.com"
```

### (Optional) Make `git pull` use rebase by default (cleaner history)
```bash
git config --global pull.rebase true
```

---

## 1) Clone the repository + initial commands

### Clone and enter the repo
```bash
git clone <REPO_URL>
cd <REPO_FOLDER>
```

### Check status and branches
```bash
git status
git branch
git branch -a
git remote -v
```

### Make sure your default branch is up to date
```bash
git switch main     # or: git switch master
git pull
```

---

## 2) Starting work on a new feature / change (create a branch)

### Step A: Update `main`
```bash
git switch main     # or: git switch master
git pull
```

### Step B: Create and switch to a new branch
Use descriptive names:
- `feature/...` for features
- `fix/...` for bugfixes
- `chore/...` for refactors/build/cleanup

```bash
git switch -c feature/<short-description>
# examples:
# git switch -c feature/add-parser
# git switch -c fix/null-crash
# git switch -c chore/cmake-cleanup
```

### Step C: Work normally (edit code), then view what changed
```bash
git status
git diff
```

### Step D (recommended): If your friend updated `main`, sync your branch
Do this occasionally for longer tasks.

**Option 1: Rebase (recommended for small teams)**
```bash
git switch main
git pull

git switch feature/<short-description>
git rebase main
```

If conflicts happen:
```bash
# fix the conflicted files manually, then:
git add -A
git rebase --continue
```

**Option 2: Merge (simpler, messier history)**
```bash
git switch main
git pull

git switch feature/<short-description>
git merge main
```

---

## 3) End of day: commit + push (upload changes)

### Step A: Review changes
```bash
git status
git diff
```

### Step B: Stage changes
Stage specific files:
```bash
git add <file1> <file2>
```

Or stage everything:
```bash
git add -A
```

### Step C: Commit
```bash
git commit -m "Short clear message about what you changed"
```

### Step D: Push your branch to the remote
First push (sets upstream):
```bash
git push -u origin feature/<short-description>
```

After that, future pushes are just:
```bash
git push
```

---

## Getting your work into `main`

### Recommended: Open a Pull Request (PR)
1. Push your branch (`git push -u origin ...`)
2. Open a PR on GitHub/GitLab from your branch → `main`
3. Merge when ready

This avoids “oops I pushed broken stuff to main”.

### Alternative: Merge directly to `main` (only if you’re careful)
This is allowed since you have no rulesets, but it’s riskier.

```bash
# Update main
git switch main
git pull

# Ensure your branch is up to date
git switch feature/<short-description>
git rebase main   # or: git merge main

# Merge into main
git switch main
git merge feature/<short-description>

# Push main
git push
```

### Cleanup old branches (after merge)
Delete locally:
```bash
git branch -d feature/<short-description>
```

Delete on remote:
```bash
git push origin --delete feature/<short-description>
```

---

## Handy commands (cheat sheet)

### See recent commits
```bash
git log --oneline --decorate --graph -20
```

### Undo staging (keep file changes)
```bash
git restore --staged .
```

### Discard local changes in a file (careful!)
```bash
git restore <file>
```

### Rename current branch
```bash
git branch -m new-branch-name
git push -u origin new-branch-name
git push origin --delete old-branch-name
```

### If you rebased and already pushed, you may need force-with-lease
Use this ONLY on your own branch (not `main`):
```bash
git push --force-with-lease
```

---

## Naming & messaging conventions (recommended)

- Branch names: `feature/<topic>`, `fix/<bug>`, `chore/<task>`
- Commit messages: start with a verb: “Add…”, “Fix…”, “Refactor…”, “Update…”

---

## Minimal daily flow (copy/paste)

### Start work
```bash
git switch main
git pull
git switch -c feature/<task>
```

### End work
```bash
git add -A
git commit -m "Describe change"
git push -u origin feature/<task>
```
