{
  description = "Flake with shell for przetwarzanie rozproszone Project";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, ... }@inputs:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.x86_64-linux.default =
        pkgs.mkShell {

          shellHook = ''
            echo "Hello in Przetwarzanie rozproszone"
          '';

          packages = (with pkgs; [
            gnumake
			ctags
			mpi
            bash
            gpp
            gcc
            clang-tools_17
            git
          ]);
        };
    };
}
