// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/devices/new_device/new_device_screen.dart';
import 'package:snoozeless/devices/devices_list_screen/devices_list.dart';
import 'package:snoozeless/shared/app_bar_button.dart';

// Third party packages
import 'package:font_awesome_flutter/font_awesome_flutter.dart';

class DevicesListScreen extends StatelessWidget {
  const DevicesListScreen({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: AppBarButton(
          icon: FontAwesomeIcons.solidUser,
          method: () {
            Navigator.pushNamed(context, '/profile');
          },
        ),
        title: const Text('Devices'),
        actions: [
          AppBarButton(
            icon: FontAwesomeIcons.plus,
            method: () {
              Navigator.of(context).push(
                MaterialPageRoute(
                  builder: (BuildContext context) => const NewDeviceScreen(),
                ),
              );
            },
          ),
        ],
      ),
      body: const DevicesList(),
    );
  }
}
