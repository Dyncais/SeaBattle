{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    nativeBuildInputs = with pkgs.buildPackages; [ 
      git 
      cmake 
      ccache 
      ninja
      raylib
      enet
      ];
    ENET_LIBRARY = "${pkgs.enet}/lib";
    ENET_INCLUDE_DIR = "${pkgs.enet}/include";
}
