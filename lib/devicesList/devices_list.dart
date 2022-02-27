import 'package:flutter/material.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'package:snoozeless/alarms/edit_device.dart';
import 'package:snoozeless/alarms/new_device.dart';
import 'package:snoozeless/devicesList/devices_list_item.dart';
import 'package:snoozeless/services/firestore.dart';
import 'package:snoozeless/services/models.dart';

class DevicesList extends StatelessWidget {
  const DevicesList({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return StreamBuilder<Iterable<Device>>(
      stream: FirestoreService().streamDevicesList(),
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.waiting) {
          return const Text('Loading');
        } else if (snapshot.hasError) {
          return const Text('Error in devices.dart');
        } else if (snapshot.hasData) {
          var devices = snapshot.data!;

          return Scaffold(
            appBar: AppBar(
              leading: IconButton(
                icon: const Icon(
                  FontAwesomeIcons.solidUser,
                  size: 20,
                ),
                alignment: Alignment.center,
                onPressed: () => Navigator.pushNamed(context, '/profile'),
              ),
              title: const Text('Devices'),
              actions: [
                IconButton(
                  icon: const Icon(
                    FontAwesomeIcons.plus,
                    size: 20,
                  ),
                  alignment: Alignment.center,
                  onPressed: () {
                    Navigator.of(context).push(
                      MaterialPageRoute(
                        builder: (BuildContext context) =>
                            const NewDeviceScreen(),
                      ),
                    );
                  },
                ),
              ],
            ),
            // Potential thread on pinning a '+' button to the bottom of the
            // ListView: https://github.com/flutter/flutter/issues/38640
            body: ListView(
              padding: const EdgeInsets.all(20),
              children: devices
                  .map((device) => DeviceListItem(device: device))
                  .toList(),
            ),
          );
        } else {
          return const Text('No devices found in Firestore. Check database.');
        }
      },
    );
  }
}
