# Naming Convention Standardisation

Constructor and destructor functions use at least three different naming schemes across the codebase with no consistent pattern. Error return values are also inconsistent in places.

**Solution:** Agree on and apply a single convention for constructors, destructors, and error returns across all subsystems. Rename one subsystem per commit to keep diffs reviewable.
