// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/alarms_screen/alarms_list_item.dart';
import 'package:snoozeless/services/models.dart';

// Third party packages
import 'package:snoozeless/services/firestore.dart';

class AlarmsList extends StatelessWidget {
  final String deviceId;
  const AlarmsList({Key? key, required this.deviceId}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(20),
      child: StreamBuilder<Iterable<Alarm>>(
        // TODO: Beautify loading and error states.
        stream: FirestoreService().streamAlarmsList(deviceId),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return const Text('Loading');
          } else if (snapshot.hasError) {
            return const Text('Error in alarms_list.dart');
          } else if (snapshot.hasData) {
            var alarms = snapshot.data!;
            return ListView(
              children:
                  alarms.map((alarm) => AlarmListItem(alarm: alarm)).toList(),
            );
          } else {
            return const Text('No alarms found in Firestore. Check database.');
          }
        },
      ),
    );
  }
}
