1. create seedless internal hash chain and get the last hash
a36a02ada10a3b0bba60f563307040246bc2b605bdc9b772930975393a1e284f

2. manually verify the last hash in the chain matches expected
a36a02ada10a3b0bba60f563307040246bc2b605bdc9b772930975393a1e284f

3. create external hash chain and verify the next internal hash is valid
pass
50d76e29b3db407d6dc8af92928d0bb5c49fe7f89f7ac76facf3cacbff067a56

4. check manually calculated next hash matches this new internal hash value
50d76e29b3db407d6dc8af92928d0bb5c49fe7f89f7ac76facf3cacbff067a56

5. verify an internal hash generated with the wrong password is not accepted
pass

6. check with limit one that external chain will not accept a now correct hash
pass

7. check with limit three the external chain now accepts the next correct hash
pass

8. check all remaining internal chain hashes are verified by the external chain
5ba23633823ee900cd4ca3a9f4ce9a08f2ca1d3920c5c44fd51273b2f7cf5ca4

9. check that the last hash matches what was manually calculated
5ba23633823ee900cd4ca3a9f4ce9a08f2ca1d3920c5c44fd51273b2f7cf5ca4
