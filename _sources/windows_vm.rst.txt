
Installing a Windows Virtual Machine with VMware
================================================

VMware Fusion vs Player vs Workstation
--------------------------------------

The instructions given here are with VMware Fusion on OSX. The steps are nearly identical for the other two VMware products.


Create a new VM
---------------

.. figure:: windows_vm/1_create_vm.jpg
	:width: 650px

	
Choose the install disk
-----------------------------

.. figure:: windows_vm/2_choose_disk.jpg
	:width: 650px


Fill out Easy-Install settings
-------------------------------

.. figure:: windows_vm/3_easy_install.jpg
	:width: 650px


Customize VM settings
---------------------

.. figure:: windows_vm/5_processors_and_memory.jpg
	:width: 650px

.. figure:: windows_vm/6_processors_and_memory.jpg
	:width: 650px
	
- This is Mac specific. VMware doesn't handle retina displays well. I recommend shutting it off here:


.. figure:: windows_vm/7_display_retina.jpg
	:width: 650px
	
Start the VM
---------------

.. figure:: windows_vm/8_start_vm.jpg
	:width: 650px

Wait for Easy-Install to complete
----------------------------------

- The VM will reboot several times. The install took about 20 minutes on my Macbook Pro. 

.. figure:: windows_vm/9_easy_install.jpg
	:width: 650px

	
Easy-Install is finished
------------------------

.. figure:: windows_vm/10_install_complete.jpg
	:width: 650px


Install Visual Studio - attaching the disk file
-----------------------------------------------

.. figure:: windows_vm/11_attach_vs_disk.jpg
	:width: 650px

- For some reason (at least in vmware fusion), the CD/DVD drive gets disabled. To turn it back on, do this:


.. figure:: windows_vm/12_turn_cd_back_on.jpg
	:width: 650px

.. figure:: windows_vm/13_turn_cd_back_on.jpg
	:width: 650px

Install Visual Studio - start the installer
-------------------------------------------

.. figure:: windows_vm/14_start_vs_installer.jpg
	:width: 650px

- I recommend not including the optional features. They won't be needed for this course:


.. figure:: windows_vm/15_optional_vs_features.jpg
	:width: 650px

- This install will take 20-30 minutes to complete:


.. figure:: windows_vm/16_vs_2013_installation.jpg
	:width: 650px


.. figure:: windows_vm/17_vs_install_complete.jpg
	:width: 650px
	

Launch and sign into Visual Studio
----------------------------------

- This step may or may not be applicable for you. With my MSDN license, I have to sign in to Visual Studio with my Microsoft account


.. figure:: windows_vm/18_vs_sign_in.jpg
	:width: 650px

	
Install VMware Player
---------------------

- The default selections are appropriate for this class.

.. figure:: windows_vm/19_vmware_player_install.jpg
	:width: 650px

	
.. figure:: windows_vm/20_vmware_player_install_complete.jpg
	:width: 650px
	

Install Mercurial
-----------------

- When you download the Mercurial installer, this is the one you want:

.. figure:: windows_vm/21_download_mercurial_installer.jpg
	:width: 650px

.. figure:: windows_vm/22_install_mercurial.jpg
	:width: 650px

.. figure:: windows_vm/23_install_mercurial_complete.jpg
	:width: 650px


Configuring and installing software and security updates
--------------------------------------------------------

- Launch an Explorer window and navigate to the Control Panel


.. figure:: windows_vm/24_goto_control_panel.jpg
	:width: 650px

	
- Change the Control Panel to show large icons:


.. figure:: windows_vm/25_configure_control_panel.jpg
	:width: 650px
	
- Go to Windows Update

.. figure:: windows_vm/26_start_windows_update.jpg
	:width: 650px

- Click the highlighted area to configure updates:


.. figure:: windows_vm/27_configure_updates.jpg
	:width: 650px

.. figure:: windows_vm/28_configure_updates.jpg
	:width: 650px

- Start the update installation process:


.. figure:: windows_vm/29_start_updates.jpg
	:width: 650px


.. figure:: windows_vm/30_install_updates.jpg
	:width: 650px


.. figure:: windows_vm/31_install_updates.jpg
	:width: 650px

	
Reboot the VM
-------------

.. figure:: windows_vm/32_restart_vm.jpg
	:width: 650px
