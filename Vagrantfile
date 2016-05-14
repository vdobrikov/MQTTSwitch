Vagrant.configure(2) do |config|
	config.vm.box = "hashicorp/precise32"

	# Enable provisioning with a shell script. Additional provisioners such as
	# Puppet, Chef, Ansible, Salt, and Docker are also available. Please see the
	# documentation for more information about their specific syntax and use.
	config.vm.provision "shell", inline: <<-SHELL
		sudo apt-get update
		sudo apt-get install python-software-properties -y
		sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
		sudo apt-get update
		sudo apt-get install g++-4.9 -y
		sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 100
		sudo apt-get install make -y
	SHELL
end
