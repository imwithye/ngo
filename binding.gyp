{
  "targets": [
    {
      "target_name": "nopher",
      "sources": [ "src/nopher.cc" ],
      "include_dirs" : ["<!(node -e \"require('nan')\")"]
    }
  ]
}