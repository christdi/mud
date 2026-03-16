# Use Designated Initialisers for Struct Construction

Constructor functions typically `calloc` a struct then set each field individually, making it unclear which fields are intentionally non-zero versus left at the `calloc` default.

**Solution:** Use C99 designated initialisers to set non-zero fields explicitly in a single compound literal assignment. Apply incrementally when constructor functions are touched for other reasons.
