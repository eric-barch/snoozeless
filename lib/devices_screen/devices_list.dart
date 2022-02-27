// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/devices_screen/devices_list_item.dart';
import 'package:snoozeless/services/models.dart';

// Third party packages
import 'package:snoozeless/services/firestore.dart';

class DevicesList extends StatelessWidget {
  const DevicesList({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(20),
      child: StreamBuilder<Iterable<Device>>(
        // TODO: Beautify loading and error states.
        stream: FirestoreService().streamDevicesList(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return const Text('Loading');
          } else if (snapshot.hasError) {
            return const Text('Error in devices.dart');
          } else if (snapshot.hasData) {
            var devices = snapshot.data!;
            return ListView(
              children: devices
                  .map((device) => DeviceListItem(device: device))
                  .toList(),
            );
          } else {
            return const Text('No devices found in Firestore. Check database.');
          }
        },
      ),
    );
  }
}
