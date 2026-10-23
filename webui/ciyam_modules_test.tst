
== the registry ================================================
ok   a module registers itself by loading
ok   - says what it is, for the console's help
ok   - a name that could not be a file's refused
ok   - named as its file is, so the server's renaming for an account's own reaches it
ok   - loading it again replaces it - no second 'class' to fail
ok   - unloaded, out of the registry

== xor_hex_data ================================================
ok   its core - a byte at a time, the shorter padded at its start
ok   an instance keeps its running result
ok   through the old globals - as the original answered

== the template, xxxxx =========================================
ok   through the old globals - as the original answered

== what a module is given ======================================
ok   SHA-256 - the browser's own where there is one
ok   - and through sha2.min.js, text as its UTF-8
ok   the sign in's hash, version 0 - a page's as the command line's, as UTF-8
ok   version 1's rounds - by the password's length in characters
ok   the sign in's hash, version 1 - the current one: hardened, a page's as Node's, as worked out here
ok   - an unknown version refused
ok   the version guessed - none kept: current; kept for the node but no versions: 0; another's kept: current; as kept
ok   - a PIN with a username after it, by its PIN
ok   a reply that says done - only "[okay]", as text or JSON
ok   progress through the page's own functions - shown at nought, hidden at the end
ok   - and other modules
ok   a module that runs nothing says it loaded

== bip39 =======================================================
ok   entropy to its words, and back - either case
ok   - nothing: twelve new words
ok   - wrong words or entropy answer with themselves and what was wrong
ok   through the old globals - loading answers with new words, and keeps them; no second usage line

== rpc_unlock ==================================================
ok   the time, then the hash of the time and the password's hash - drawn as a QR code
ok   - its usage hint the command with the answer in it

== harden ======================================================
ok   in steps - primed, rounds, rounds repeated, then ready again
ok   - '(ready)' leaves the result kept before
ok   'reset' kept where a list waits on it - not ready once primed, ready after a repeat
ok   let go while running - it stops, says nothing and keeps nothing on the page
ok   stopped by the page - nothing said, nothing kept, and the next call afresh
ok   unloaded - out of the registry, nothing left to stop

== chacha20 ====================================================
ok   as crypt -cc - 200 zero bytes, the keystream itself
ok   - a message, and back with the same call
ok   - 70000 bytes, past its buffer
ok   the standard's own test - RFC 8439
ok   through run - '<password> <hex>', and again to decrypt
ok   - a password alone, or data not hex, said

== ed25519 =====================================================
ok   the standard's public keys - RFC 8032
ok   - its signatures, verified
ok   - another message not verified
ok   the shared secret - the same from either side, as the console's
ok   - a public key of small order refused
ok   secrets read as the console reads them
ok   - and a shared secret between two of them, as the console's
ok   - what the console refuses, refused
ok   - and what it takes with filler, the same keys
ok   through run - public, shared, sign and verify
ok   - anything else said

== secure_message ==============================================
ok   HMAC-SHA256 - the standard's tests, RFC 4231
ok   - the same through sha2.min.js, with no SHA-256 of the browser's
ok   the own secret - as the console's, from the entropy and it reversed
ok   - entropy not 32 to 64 hex digits, said
ok   sealed - as the console seals it, given its salt and padding
ok   - and opened
ok   sealed afresh - salt, ciphertext and HMAC, padded to 127 bytes, opened again with CR as a line break
ok   - each time a new salt and padding
ok   not opened - changed, another secret, not sealed, or too long to seal
ok   - 90 bytes sealed, the most
ok   to another person - the public keys and the secret between them, as the console's, from either side
ok   - sealed by one, opened by the other, and not under either's own secret
ok   - a public key not a key, said
ok   through run - own, seal and open, CRs shown as \r
ok   - public and shared
ok   - anything else said

== user_keys ===================================================
ok   the twelve words for entropy and back - BIP39, as a node's
ok   - words not right, said
ok   locked - as worked out without the module, given a salt and rounds
ok   - and unlocked
ok   locked afresh - its own salt each time, the default rounds, unlocked again
ok   not unlocked - another password, changed, or not locked keys
ok   not locked - no password, entropy not hex, a public key not one
ok   - more rounds than ten times the default refused, before any hashing; odd entropy refused
ok   - a password the same whichever Unicode form it was typed in
ok   through run - lock, unlock, words and entropy
ok   - anything else said

All checks passed.
