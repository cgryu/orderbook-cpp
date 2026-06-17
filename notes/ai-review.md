Used a CLI agent (claude code opus 4.8) to review the codebase - 

Learning points:
- Be careful about mechanical correctness
- Clearly define scope and code around it
- It's helpful for seeing actual bugs, but make own decisions on structure

Things to change based on AI Code Review: 

- Fix bug(a), add some checker for duplicate ID
- Fix bug(d), keep types for ID consistent
- Clean up notes I left for myself

Things not to change from AI Code Review: 

- Scaling of std::map and std::list (whole point of project is to test different data structures)
- Acceptance of qty < 0 isn't necessarily a bug, keep it as is and address later
- Fixing bug(a) addresses bug(c) because bug(c) happens because of bug(a) 