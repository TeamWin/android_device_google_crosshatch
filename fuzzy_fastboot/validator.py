'''
This is an example validator to be used with oem commands that allow you to
upload data afterwards that you wish to validate locally.
'''
import sys
import os
import hashlib

def eprint(msg):
  '''
  A helper function for logging error messages to fuzzy_fastboot
  Use this function as you would "print()"
  '''
  sys.stderr.write(msg + '\n')


def main():
  '''
  Data is sent back to the parent fuzzy_fastboot process through the stderr pipe.

  If this script has a non-zero return code, anything written to STDERR is part of
  the error message that will logged by FF to explain why this validation failed.

  Feel free to print to to STDOUT with print() as usual to print info to console
  '''
  if len(sys.argv) != 3:
    eprint("This script is intended to be called by fuzzy_fastboot")
    return -1

  script, command, fname = sys.argv

  with open(fname, "rb") as fd:
    buf = fd.read()
    sha = buf[0x10:0x30]
    tlvs = buf[0x30:]
    hash = hashlib.sha256(tlvs)
    # Assert the hash matches
    pretty = "".join("{:02x}".format(ord(c)) for c in sha)
    if hash.digest() != sha:
      eprint("'%s' does not match '%s'" % (hash.hexdigest(), pretty))
    assert hash.digest() == sha

  # non-zero return code signals error
  return 0


if __name__ == "__main__":
  sys.exit(main())
